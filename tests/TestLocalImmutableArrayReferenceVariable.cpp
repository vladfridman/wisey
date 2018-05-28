//
//  TestLocalImmutableArrayReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalImmutableArrayReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/LocalImmutableArrayReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalImmutableArrayReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  const wisey::ArrayType* mArrayType;
  const wisey::ArrayType* mAnotherArrayType;
  const ImmutableArrayType* mImmutableArrayType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalImmutableArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    mImmutableArrayType = mArrayType->getImmutable();
    mAnotherArrayType = mContext.getArrayType(PrimitiveTypes::FLOAT, 1u);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
  }
  
  ~LocalImmutableArrayReferenceVariableTest() {
  }
};

TEST_F(LocalImmutableArrayReferenceVariableTest, basicFieldsTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayReferenceVariable variable("foo", mImmutableArrayType, alloc, 0);
  
  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mImmutableArrayType, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(LocalImmutableArrayReferenceVariableTest, generateIdentifierIRTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayReferenceVariable variable("foo", mImmutableArrayType, alloc, 0);
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

TEST_F(LocalImmutableArrayReferenceVariableTest, generateIdentifierReferenceIRTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayReferenceVariable variable("foo", mImmutableArrayType, alloc, 0);
  
  EXPECT_EQ(alloc, variable.generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(LocalImmutableArrayReferenceVariableTest, generateWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayReferenceVariable variable("foo", mImmutableArrayType, alloc, 0);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mImmutableArrayType));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mImmutableArrayType));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i8*"
  "\n  call void @__adjustReferenceCounterForImmutableArray(i8* %1, i64 -1)"
  "\n  %2 = bitcast { i64, i64, i64, [0 x i32] }* null to i8*"
  "\n  call void @__adjustReferenceCounterForImmutableArray(i8* %2, i64 1)"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalImmutableArrayReferenceVariableTest, generateWholeArrayAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayReferenceVariable variable("foo", mImmutableArrayType, alloc, 0);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mAnotherArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type float[] to immutable int[]\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(LocalImmutableArrayReferenceVariableTest, generateArrayElementAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayReferenceVariable variable("foo", mImmutableArrayType, alloc, 0);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  arrayIndices.push_back(&mockExpression);
  
  Mock::AllowLeak(&mockExpression);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Attempting to change a value in an immutable array\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, localImmutableArrayReferenceRunTest) {
  runFile("tests/samples/test_local_immutable_array_reference.yz", 3);
}

TEST_F(TestFileRunner, localImmutableArrayReferenceRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_immutable_array_reference_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_immutable_array_reference_rce.yz:12)\n"
                               "Details: Object referenced by expression of type immutable int[][]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localImmutableArrayReferenceElementAssignRunDeathTest) {
  expectFailCompile("tests/samples/test_local_immutable_array_reference_element_assign.yz",
                    1,
                    "tests/samples/test_local_immutable_array_reference_element_assign.yz\\(12\\): "
                    "Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, localImmutableArrayReferenceCastFromArrayReferenceRunDeathTest) {
  expectFailCompile("tests/samples/test_local_immutable_array_reference_cast_from_array_reference.yz",
                    1,
                    "tests/samples/test_local_immutable_array_reference_cast_from_array_reference.yz\\(10\\): "
                    "Error: Incompatible types: can not cast from type int\\[\\]\\[\\] to immutable int\\[\\]\\[\\]");
}

TEST_F(TestFileRunner, localImmutableArrayReferenceCastFromArrayOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_local_immutable_array_reference_cast_from_array_owner.yz",
                    1,
                    "tests/samples/test_local_immutable_array_reference_cast_from_array_owner.yz\\(10\\): "
                    "Error: Incompatible types: can not cast from type int\\[\\]\\[\\]\\* to immutable int\\[\\]\\[\\]");
}
