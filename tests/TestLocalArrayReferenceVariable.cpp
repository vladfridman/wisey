//
//  TestLocalArrayReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalArrayReferenceVariable}
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
#include "LocalArrayReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalArrayReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  Function* mFunction;
  const wisey::ArrayType* mArrayType;
  const wisey::ArrayType* mAnotherArrayType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    mAnotherArrayType = mContext.getArrayType(PrimitiveTypes::FLOAT, 1u);
    
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
  
  ~LocalArrayReferenceVariableTest() {
  }
};

TEST_F(LocalArrayReferenceVariableTest, basicFieldsTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc, 0);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mArrayType, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isStatic());
}

TEST_F(LocalArrayReferenceVariableTest, generateIdentifierIRTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc, 0);
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

TEST_F(LocalArrayReferenceVariableTest, generateIdentifierReferenceIRTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc, 0);
  
  EXPECT_EQ(alloc, variable.generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(LocalArrayReferenceVariableTest, generateWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc, 0);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mArrayType));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);

  *mStringStream << *mFunction;

  string expected =
  "\ndefine internal i32 @test() {"
  "\ndeclare:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = icmp eq { i64, i64, i64, [0 x i32] }* %0, null"
  "\n  br i1 %1, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %2 = icmp eq { i64, i64, i64, [0 x i32] }* null, null"
  "\n  br i1 %2, label %if.end1, label %if.notnull2"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %3 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i64*"
  "\n  %count = load i64, i64* %3"
  "\n  %4 = add i64 %count, -1"
  "\n  store i64 %4, i64* %3"
  "\n  br label %if.end"
  "\n"
  "\nif.end1:                                          ; preds = %if.notnull2, %if.end"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo"
  "\n"
  "\nif.notnull2:                                      ; preds = %if.end"
  "\n  %5 = bitcast { i64, i64, i64, [0 x i32] }* null to i64*"
  "\n  %count3 = load i64, i64* %5"
  "\n  %6 = add i64 %count3, 1"
  "\n  store i64 %6, i64* %5"
  "\n  br label %if.end1"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayReferenceVariableTest, generateWholeArrayAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc, 0);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mAnotherArrayType));

  Mock::AllowLeak(&mockExpression);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type float[] to int[]\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, arrayReferenceOfIntsRunTest) {
  runFile("tests/samples/test_array_reference_of_ints.yz", 5);
}

TEST_F(TestFileRunner, arrayReferenceOfIntsAutocastRunTest) {
  runFile("tests/samples/test_array_reference_of_ints_autocast.yz", 1);
}

TEST_F(TestFileRunner, arrayReferenceOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_array_reference_of_ints_increment_element.yz", 5);
}

TEST_F(TestFileRunner, arrayReferenceOfModelOwnersRunTest) {
  runFile("tests/samples/test_array_reference_of_model_owners.yz", 2018);
}

TEST_F(TestFileRunner, arrayReferenceOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_array_reference_of_model_owners.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, arrayReferenceElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_reference_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_reference_element_is_nulled_on_owner_transfer.yz:19)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner,
       ownerVariableIsNulledOnOwnershipTransferToArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_reference_element.yz:18)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, ownerArrayReferenceElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_array_reference_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_array_reference_elements_initialized_to_null.yz:16)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, arrayReferenceOfModelReferencesRunTest) {
  runFile("tests/samples/test_array_reference_of_model_references.yz", 2018);
}

TEST_F(TestFileRunner, referenceCountDecrementsOnArrayReferenceElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_array_reference_element_unassign.yz", 5);
}

TEST_F(TestFileRunner, referenceCountIncrementsOnAssignToArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_array_reference_element.yz:17)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, referenceArrayReferenceInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_array_reference_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_array_reference_initialized_to_null.yz:16)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localArrayReferenceOfModelReferencesOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_reference_of_model_references_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_reference_of_model_references_out_of_bounds.yz:14)\n"
                               "Details: Index -1 is used on array of size 5\n"
                               "Main thread ended without a result\n");
}

