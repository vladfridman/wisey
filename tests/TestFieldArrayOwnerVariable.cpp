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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "ArrayType.hpp"
#include "FieldArrayOwnerVariable.hpp"
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

struct FieldArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  const wisey::ArrayType* mArrayType;
  FieldArrayOwnerVariable* mFieldArrayOwnerVariable;
  Controller* mObject;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  FieldArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);

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
    types.push_back(mArrayType->getOwner()->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mArrayType->getOwner(), "foo", 0));
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

    mFieldArrayOwnerVariable = new FieldArrayOwnerVariable("foo", mObject, 0);
  }
  
  ~FieldArrayOwnerVariableTest() {
  }
};

TEST_F(FieldArrayOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldArrayOwnerVariable->getName().c_str());
  EXPECT_EQ(mArrayType->getOwner(), mFieldArrayOwnerVariable->getType());
  EXPECT_TRUE(mFieldArrayOwnerVariable->isField());
  EXPECT_FALSE(mFieldArrayOwnerVariable->isStatic());
}

TEST_F(FieldArrayOwnerVariableTest, generateIdentifierIRTest) {
  mFieldArrayOwnerVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %foo = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayOwnerVariableTest, generateIdentifierReferenceIRTest) {
  mFieldArrayOwnerVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1\n";
  
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
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @test() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0"
  "\n  %2 = bitcast { i64, i64, i64, [0 x i32] }* %1 to i64*"
  "\n  invoke void @__destroyPrimitiveArrayFunction(i64* %2, i64 1, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @\"int[]*\", i32 0, i32 0), i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %3 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %4 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %3, { i8*, i32 }* %4"
  "\n  %5 = getelementptr { i8*, i32 }, { i8*, i32 }* %4, i32 0, i32 0"
  "\n  %6 = load i8*, i8** %5"
  "\n  %7 = call i8* @__cxa_get_exception_ptr(i8* %6)"
  "\n  %8 = getelementptr i8, i8* %7, i64 8"
  "\n  %9 = icmp eq %systems.vos.wisey.compiler.tests.CObject* null, null"
  "\n  br i1 %9, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %cleanup.cont"
  "\n  resume { i8*, i32 } %3"
  "\n"
  "\nif.notnull:                                       ; preds = %cleanup.cont"
  "\n  %10 = bitcast %systems.vos.wisey.compiler.tests.CObject* null to i64*"
  "\n  %11 = getelementptr i64, i64* %10, i64 -1"
  "\n  %count = load i64, i64* %11"
  "\n  %12 = add i64 %count, -1"
  "\n  store i64 %12, i64* %11"
  "\n  br label %if.end"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %0"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldArrayOwnerVariableTest, generateWholeArrayAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mFieldArrayOwnerVariable->generateAssignmentIR(mContext,
                                                                  &mockExpression,
                                                                  arrayIndices,
                                                                  5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type int[] to int[]*\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, fieldArrayOwnerOfIntsRunTest) {
  runFile("tests/samples/test_field_array_owner_of_ints.yz", 5);
}

TEST_F(TestFileRunner, fieldArrayOwnerOfIntsAutocastRunTest) {
  runFile("tests/samples/test_field_array_owner_of_ints_autocast.yz", 1);
}

TEST_F(TestFileRunner, fieldArrayOwnerOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_field_array_owner_of_ints_increment_element.yz", 5);
}

TEST_F(TestFileRunner, fieldArrayOwnerOfModelOwnersRunTest) {
  runFile("tests/samples/test_field_array_owner_of_model_owners.yz", 2018);
}

TEST_F(TestFileRunner, fieldArrayOwnerOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_field_array_owner_of_model_owners.yz",
                                        "destructor systems.vos.wisey.compiler.tests.CController\n"
                                        "freeing systems.vos.wisey.compiler.tests.CController.mArray\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CController\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, fieldArrayOwnerElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_owner_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_field_array_owner_element_is_nulled_on_owner_transfer.yz:24)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_owner_element_is_nulled_on_owner_transfer.yz:35)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner,
       ownerVariableIsNulledOnOwnershipTransferToFieldArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_owner_element.yz:18)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_field_array_owner_element.yz:27)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, ownerFieldArrayOwnerElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_field_array_owner_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_owner_field_array_owner_elements_initialized_to_null.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_field_array_owner_elements_initialized_to_null.yz:25)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldArrayOwnerOfModelReferencesRunTest) {
  runFile("tests/samples/test_field_array_owner_of_model_references.yz", 2018);
}

TEST_F(TestFileRunner, referenceCountDecrementsOnFieldArrayOwnerElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_field_array_owner_element_unassign.yz", 5);
}

TEST_F(TestFileRunner, referenceCountIncrementsOnAssignToFieldArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_field_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_field_array_owner_element.yz:29)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, referenceFieldArrayOwnerInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_field_array_owner_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getCarYear(tests/samples/test_reference_field_array_owner_initialized_to_null.yz:15)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_field_array_owner_initialized_to_null.yz:24)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldArrayOfIntsRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_of_ints_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getRce(tests/samples/test_field_array_of_ints_rce.yz:10)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_of_ints_rce.yz:19)\n"
                               "Details: Object referenced by expression of type int[]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldArrayOfOwnersRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_of_owners_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getRce(tests/samples/test_field_array_of_owners_rce.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_of_owners_rce.yz:25)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar*[]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldArrayOfReferencesRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_of_references_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getRce(tests/samples/test_field_array_of_references_rce.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_of_references_rce.yz:25)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar[]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, modelReturnsArrayOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_model_returns_array_owner.yz",
                    1,
                    "tests/samples/test_model_returns_array_owner.yz\\(9\\): Error: "
                    "Setting an unassignable owner field 'mArray' of object systems.vos.wisey.compiler.tests.MElement to null possibly by returning its value");
}
