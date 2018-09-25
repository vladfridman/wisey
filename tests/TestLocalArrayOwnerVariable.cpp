//
//  TestLocalArrayOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalArrayOwnerVariable}
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
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "IntConstant.hpp"
#include "LocalArrayOwnerVariable.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  Function* mFunction;
  const wisey::ArrayType* mArrayType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  LocalArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
  }
  
  ~LocalArrayOwnerVariableTest() {
  }
};

TEST_F(LocalArrayOwnerVariableTest, basicFieldsTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc, 0);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mArrayType->getOwner(), variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isStatic());
}

TEST_F(LocalArrayOwnerVariableTest, generateIdentifierIRTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc, 0);
  variable.generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalArrayOwnerVariableTest, generateIdentifierReferenceIRTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc, 0);

  EXPECT_EQ(alloc, variable.generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(LocalArrayOwnerVariableTest, generateAssignmentTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc, 0);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType->getOwner()));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mArrayType->getOwner()));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @test() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i64*"
  "\n  invoke void @__destroyPrimitiveArrayFunction(i64* %1, i64 1, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @\"int[]*\", i32 0, i32 0), i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %2 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %3 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %2, { i8*, i32 }* %3"
  "\n  %4 = getelementptr { i8*, i32 }, { i8*, i32 }* %3, i32 0, i32 0"
  "\n  %5 = load i8*, i8** %4"
  "\n  %6 = call i8* @__cxa_get_exception_ptr(i8* %5)"
  "\n  %7 = getelementptr i8, i8* %6, i64 8"
  "\n  resume { i8*, i32 } %2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayOwnerVariableTest, generateAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc, 0);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type int[] to int[]*\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, arrayOwnerOfIntsRunTest) {
  runFile("tests/samples/test_array_owner_of_ints.yz", 5);
}

TEST_F(TestFileRunner, arrayOwnerOfIntsAutocastRunTest) {
  runFile("tests/samples/test_array_owner_of_ints_autocast.yz", 1);
}

TEST_F(TestFileRunner, arrayOwnerOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_array_owner_of_ints_increment_element.yz", 5);
}

TEST_F(TestFileRunner, arrayOwnerOfModelOwnersRunTest) {
  runFile("tests/samples/test_array_owner_of_model_owners.yz", 2018);
}

TEST_F(TestFileRunner, arrayOwnerOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_array_owner_of_model_owners.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, arrayOwnerElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_owner_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_owner_element_is_nulled_on_owner_transfer.yz:19)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner,
       ownerVariableIsNulledOnOwnershipTransferToArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_owner_element.yz:18)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, ownerArrayOwnerElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_array_owner_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_array_owner_elements_initialized_to_null.yz:16)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, arrayOwnerOfModelReferencesRunTest) {
  runFile("tests/samples/test_array_owner_of_model_references.yz", 2018);
}

TEST_F(TestFileRunner, referenceCountDecrementsOnArrayOwnerElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_array_owner_element_unassign.yz", 5);
}

TEST_F(TestFileRunner, referenceCountIncrementsOnAssignToArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_array_owner_element.yz:17)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, referenceArrayOwnerInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_array_owner_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_array_owner_initialized_to_null.yz:16)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localArrayOfIntsRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_of_ints_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_of_ints_rce.yz:10)\n"
                               "Details: Object referenced by expression of type int[][]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localArrayOfOwnersRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_of_owners_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_of_owners_rce.yz:14)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar*[]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localArrayOfReferencesRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_of_references_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_of_references_rce.yz:14)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar[]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localArrayOwnerOfIntsOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_owner_of_ints_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_owner_of_ints_out_of_bounds.yz:10)\n"
                               "Details: Index 2 is used on array of size 2\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localArrayOwnerOfInts2dOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_owner_of_ints_2d_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_owner_of_ints_2d_out_of_bounds.yz:10)\n"
                               "Details: Index 6 is used on array of size 5\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localArrayOwnerOfModelOwnersOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_owner_of_model_owners_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_owner_of_model_owners_out_of_bounds.yz:13)\n"
                               "Details: Index 5 is used on array of size 5\n"
                               "Main thread ended without a result\n");
}
