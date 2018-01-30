//
//  TestFieldArrayOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldArrayOwnerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/FieldArrayOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  FieldArrayOwnerVariable* mFieldArrayOwnerVariable;
  Controller* mObject;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  FieldArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<unsigned long> dimensions;
    dimensions.push_back(3u);
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, dimensions);

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
    types.push_back(mArrayType->getOwner()->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList fieldArguments;
    fields.push_back(new Field(STATE_FIELD, mArrayType->getOwner(), "foo", fieldArguments));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType);
    mObject->setFields(fields, 1u);
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);

    mFieldArrayOwnerVariable = new FieldArrayOwnerVariable("foo", mObject);
  }
  
  ~FieldArrayOwnerVariableTest() {
  }
};

TEST_F(FieldArrayOwnerVariableTest, getNameTest) {
  EXPECT_STREQ("foo", mFieldArrayOwnerVariable->getName().c_str());
}

TEST_F(FieldArrayOwnerVariableTest, getTypeTest) {
  EXPECT_EQ(mArrayType->getOwner(), mFieldArrayOwnerVariable->getType());
}

TEST_F(FieldArrayOwnerVariableTest, generateIdentifierIRTest) {
  mFieldArrayOwnerVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, [3 x i32] }*, { i64, i64, [3 x i32] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayOwnerVariableTest, generateWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType->getOwner()));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mArrayType->getOwner()));
  mFieldArrayOwnerVariable->generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, [3 x i32] }*, { i64, i64, [3 x i32] }** %0"
  "\n  %2 = bitcast { i64, i64, [3 x i32] }* %1 to i64*"
  "\n  %3 = call i64 @__destroyPrimitiveArrayFunction(i64* %2, i64 1, i64 4, i1 true)"
  "\n  store { i64, i64, [3 x i32] }* null, { i64, i64, [3 x i32] }** %0\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayOwnerVariableTest, generateWholeArrayAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(mFieldArrayOwnerVariable->generateAssignmentIR(mContext,
                                                             &mockExpression,
                                                             arrayIndices,
                                                             0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'int\\[3\\]' to 'int\\[3\\]\\*'");
}

TEST_F(TestFileSampleRunner, fieldArrayOwnerOfIntsRunTest) {
  runFile("tests/samples/test_field_array_owner_of_ints.yz", "5");
}

TEST_F(TestFileSampleRunner, fieldArrayOwnerOfIntsAutocastRunTest) {
  runFile("tests/samples/test_field_array_owner_of_ints_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, fieldArrayOwnerOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_field_array_owner_of_ints_increment_element.yz", "5");
}

TEST_F(TestFileSampleRunner, fieldArrayOwnerOfModelOwnersRunTest) {
  runFile("tests/samples/test_field_array_owner_of_model_owners.yz", "2018");
}

TEST_F(TestFileSampleRunner, fieldArrayOwnerOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_field_array_owner_of_model_owners.yz",
                                        "destructor systems.vos.wisey.compiler.tests.CController\n"
                                        "destructor <object>*[5]\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileSampleRunner, fieldArrayOwnerElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_owner_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_field_array_owner_element_is_nulled_on_owner_transfer.yz:24)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_owner_element_is_nulled_on_owner_transfer.yz:35)\n");
}

TEST_F(TestFileSampleRunner,
       ownerVariableIsNulledOnOwnershipTransferToFieldArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_owner_element.yz:18)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_owner_element.yz:27)\n");
}

TEST_F(TestFileSampleRunner, ownerFieldArrayOwnerElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_field_array_owner_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_field_array_owner_elements_initialized_to_null.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_field_array_owner_elements_initialized_to_null.yz:25)\n");
}

TEST_F(TestFileSampleRunner, fieldArrayOwnerOfModelReferencesRunTest) {
  runFile("tests/samples/test_field_array_owner_of_model_references.yz", "2018");
}

TEST_F(TestFileSampleRunner, referenceCountDecrementsOnFieldArrayOwnerElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_field_array_owner_element_unassign.yz", "5");
}

TEST_F(TestFileSampleRunner, referenceCountIncrementsOnAssignToFieldArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_field_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_field_array_owner_element.yz:29)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, referenceFieldArrayOwnerInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_field_array_owner_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_reference_field_array_owner_initialized_to_null.yz:15)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_field_array_owner_initialized_to_null.yz:24)\n");
}

TEST_F(TestFileSampleRunner, fieldArrayOfIntsRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_of_ints_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getRce(tests/samples/test_field_array_of_ints_rce.yz:10)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_of_ints_rce.yz:19)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, fieldArrayOfOwnersRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_of_owners_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getRce(tests/samples/test_field_array_of_owners_rce.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_of_owners_rce.yz:25)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, fieldArrayOfReferencesRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_of_references_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getRce(tests/samples/test_field_array_of_references_rce.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_of_references_rce.yz:25)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}
