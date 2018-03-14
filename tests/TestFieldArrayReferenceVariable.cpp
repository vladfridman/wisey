//
//  TestFieldArrayReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldArrayReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/FieldArrayReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldArrayReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  const wisey::ArrayType* mAnotherArrayType;
  FieldArrayReferenceVariable* mFieldArrayReferenceVariable;
  Controller* mObject;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  FieldArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<unsigned long> dimensions;
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
    mAnotherArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 2u);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(mArrayType->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mArrayType, "foo"));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType);
    mObject->setFields(fields, 1u);
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mFieldArrayReferenceVariable = new FieldArrayReferenceVariable("foo", mObject);
  }
  
  ~FieldArrayReferenceVariableTest() {
  }
};

TEST_F(FieldArrayReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldArrayReferenceVariable->getName().c_str());
  EXPECT_EQ(mArrayType, mFieldArrayReferenceVariable->getType());
  EXPECT_TRUE(mFieldArrayReferenceVariable->isField());
  EXPECT_FALSE(mFieldArrayReferenceVariable->isSystem());
}

TEST_F(FieldArrayReferenceVariableTest, generateIdentifierIRTest) {
  mFieldArrayReferenceVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayReferenceVariableTest, generateIdentifierReferenceIRTest) {
  mFieldArrayReferenceVariable->generateIdentifierReferenceIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayReferenceVariableTest, generateWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mArrayType));
  mFieldArrayReferenceVariable->generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0"
  "\n  %2 = bitcast { i64, i64, i64, [0 x i32] }* %1 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %2, i64 -1)"
  "\n  %3 = bitcast { i64, i64, i64, [0 x i32] }* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %3, i64 1)"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %0\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayReferenceVariableTest, generateWholeArrayAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mAnotherArrayType));

  Mock::AllowLeak(&mockExpression);

  EXPECT_EXIT(mFieldArrayReferenceVariable->generateAssignmentIR(mContext,
                                                                 &mockExpression,
                                                                 arrayIndices,
                                                                 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'int\\[\\]\\[\\]' to 'int\\[\\]\\'");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceOfIntsRunTest) {
  runFile("tests/samples/test_field_array_reference_of_ints.yz", "5");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceOfIntsAutocastRunTest) {
  runFile("tests/samples/test_field_array_reference_of_ints_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_field_array_reference_of_ints_increment_element.yz", "5");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceOfModelOwnersRunTest) {
  runFile("tests/samples/test_field_array_reference_of_model_owners.yz", "2018");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_field_array_reference_of_model_owners.yz",
                                        "destructor systems.vos.wisey.compiler.tests.CController\n"
                                        "destructor object<8 bytes>*[5]\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_reference_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_field_array_reference_element_is_nulled_on_owner_transfer.yz:24)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_reference_element_is_nulled_on_owner_transfer.yz:36)\n");
}

TEST_F(TestFileSampleRunner,
       ownerVariableIsNulledOnOwnershipTransferToFieldArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_reference_element.yz:18)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_reference_element.yz:28)\n");
}

TEST_F(TestFileSampleRunner, ownerFieldArrayReferenceElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_field_array_reference_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_field_array_reference_elements_initialized_to_null.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_field_array_reference_elements_initialized_to_null.yz:26)\n");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceOfModelReferencesRunTest) {
  runFile("tests/samples/test_field_array_reference_of_model_references.yz", "2018");
}

TEST_F(TestFileSampleRunner, referenceCountDecrementsOnFieldArrayReferenceElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_field_array_reference_element_unassign.yz", "5");
}

TEST_F(TestFileSampleRunner, referenceCountIncrementsOnAssignToFieldArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_field_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_field_array_reference_element.yz:29)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, referenceFieldArrayReferenceInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_field_array_reference_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_reference_field_array_reference_initialized_to_null.yz:15)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_field_array_reference_initialized_to_null.yz:25)\n");
}

