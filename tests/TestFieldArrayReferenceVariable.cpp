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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "ArrayType.hpp"
#include "FieldArrayReferenceVariable.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "IntConstant.hpp"
#include "ParameterReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "StateField.hpp"

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
  Function* mFunction;
  const wisey::ArrayType* mArrayType;
  const wisey::ArrayType* mAnotherArrayType;
  FieldArrayReferenceVariable* mFieldArrayReferenceVariable;
  Controller* mObject;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  FieldArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<unsigned long> dimensions;
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    mAnotherArrayType = mContext.getArrayType(PrimitiveTypes::INT, 2u);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(mArrayType->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mArrayType, "foo", 0));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType,
                                        mContext.getImportProfile(),
                                        0);
    mObject->setFields(mContext, fields, 1u);
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer,
                                                             0);
    mContext.getScopes().setVariable(mContext, thisVariable);
    
    mFieldArrayReferenceVariable = new FieldArrayReferenceVariable("foo", mObject, 0);
  }
  
  ~FieldArrayReferenceVariableTest() {
  }
};

TEST_F(FieldArrayReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldArrayReferenceVariable->getName().c_str());
  EXPECT_EQ(mArrayType, mFieldArrayReferenceVariable->getType());
  EXPECT_TRUE(mFieldArrayReferenceVariable->isField());
  EXPECT_FALSE(mFieldArrayReferenceVariable->isStatic());
}

TEST_F(FieldArrayReferenceVariableTest, generateIdentifierIRTest) {
  mFieldArrayReferenceVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayReferenceVariableTest, generateIdentifierReferenceIRTest) {
  mFieldArrayReferenceVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
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
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @test() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0"
  "\n  %2 = icmp eq { i64, i64, i64, [0 x i32] }* %1, null"
  "\n  br i1 %2, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %3 = icmp eq { i64, i64, i64, [0 x i32] }* null, null"
  "\n  br i1 %3, label %if.end1, label %if.notnull2"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %4 = bitcast { i64, i64, i64, [0 x i32] }* %1 to i64*"
  "\n  %count = load i64, i64* %4"
  "\n  %5 = add i64 %count, -1"
  "\n  store i64 %5, i64* %4"
  "\n  br label %if.end"
  "\n"
  "\nif.end1:                                          ; preds = %if.notnull2, %if.end"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %0"
  "\n"
  "\nif.notnull2:                                      ; preds = %if.end"
  "\n  %6 = bitcast { i64, i64, i64, [0 x i32] }* null to i64*"
  "\n  %count3 = load i64, i64* %6"
  "\n  %7 = add i64 %count3, 1"
  "\n  store i64 %7, i64* %6"
  "\n  br label %if.end1"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayReferenceVariableTest, generateWholeArrayAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mAnotherArrayType));

  Mock::AllowLeak(&mockExpression);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mFieldArrayReferenceVariable->generateAssignmentIR(mContext,
                                                                      &mockExpression,
                                                                      arrayIndices,
                                                                      5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type int[][] to int[]\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, fieldArrayReferenceOfIntsRunTest) {
  runFile("tests/samples/test_field_array_reference_of_ints.yz", 5);
}

TEST_F(TestFileRunner, fieldArrayReferenceOfIntsAutocastRunTest) {
  runFile("tests/samples/test_field_array_reference_of_ints_autocast.yz", 1);
}

TEST_F(TestFileRunner, fieldArrayReferenceOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_field_array_reference_of_ints_increment_element.yz", 5);
}

TEST_F(TestFileRunner, fieldArrayReferenceOfModelOwnersRunTest) {
  runFile("tests/samples/test_field_array_reference_of_model_owners.yz", 2018);
}

TEST_F(TestFileRunner, fieldArrayReferenceOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_field_array_reference_of_model_owners.yz",
                                        "destructor systems.vos.wisey.compiler.tests.CController\n"
                                        "decrementing systems.vos.wisey.compiler.tests.CController.mArray\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CController\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, fieldArrayReferenceElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_reference_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_field_array_reference_element_is_nulled_on_owner_transfer.yz:24)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_reference_element_is_nulled_on_owner_transfer.yz:36)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner,
       ownerVariableIsNulledOnOwnershipTransferToFieldArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_reference_element.yz:18)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_reference_element.yz:28)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, ownerFieldArrayReferenceElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_field_array_reference_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_field_array_reference_elements_initialized_to_null.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_field_array_reference_elements_initialized_to_null.yz:26)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldArrayReferenceOfModelReferencesRunTest) {
  runFile("tests/samples/test_field_array_reference_of_model_references.yz", 2018);
}

TEST_F(TestFileRunner, referenceCountDecrementsOnFieldArrayReferenceElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_field_array_reference_element_unassign.yz", 5);
}

TEST_F(TestFileRunner, referenceCountIncrementsOnAssignToFieldArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_field_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_field_array_reference_element.yz:29)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, referenceFieldArrayReferenceInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_field_array_reference_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_reference_field_array_reference_initialized_to_null.yz:15)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_field_array_reference_initialized_to_null.yz:25)\n"
                               "Main thread ended without a result\n");
}

