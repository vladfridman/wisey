//
//  TestIRWriter.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IRWriter}

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct IRWriterTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Function* mMainFunction;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IRWriterTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    mMainFunction = Function::Create(functionType,
                                     GlobalValue::InternalLinkage,
                                     "main",
                                     mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mMainFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mContext.setMainFunction(mMainFunction);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~IRWriterTest() {
    delete mStringStream;
  }
};

TEST_F(IRWriterTest, createReturnInstTest) {
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ReturnInst* returnInst = IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *returnInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  ret i32 1");

  value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 2);
  IRWriter::createReturnInst(mContext, value);

  EXPECT_EQ(mBasicBlock->size(), 1u);
  EXPECT_EQ(mBasicBlock->getTerminator(), returnInst);
}

TEST_F(IRWriterTest, createBranchTest) {
  BasicBlock* block1 = BasicBlock::Create(mLLVMContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(mLLVMContext, "block2");
  
  BranchInst* branchInst = IRWriter::createBranch(mContext, block1);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *branchInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  br label %block1");

  IRWriter::createBranch(mContext, block2);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
}

TEST_F(IRWriterTest, createConditionalBranchTest) {
  BasicBlock* block1 = BasicBlock::Create(mLLVMContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(mLLVMContext, "block2");
  Value* conditionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  
  BranchInst* branchInst =
  IRWriter::createConditionalBranch(mContext, block1, block2, conditionValue);

  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *branchInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %block1, label %block2");
  
  IRWriter::createConditionalBranch(mContext, block1, block2, conditionValue);

  EXPECT_EQ(mBasicBlock->size(), 1u);
}

TEST_F(IRWriterTest, createBinaryOperator) {
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  BinaryOperator* binaryOperator =
  IRWriter::createBinaryOperator(mContext, Instruction::Add, value, value, "");

  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *binaryOperator;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %0 = add i32 1, 1");

  IRWriter::createReturnInst(mContext, value);

  EXPECT_EQ(mBasicBlock->size(), 2u);

  IRWriter::createBinaryOperator(mContext, Instruction::Add, value, value, "");

  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, createCallInstTest) {
  vector<Value*> arguments;
  CallInst* callInst = IRWriter::createCallInst(mContext, mMainFunction, arguments, "");

  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *callInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %0 = call i64 @main()");

  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);

  IRWriter::createCallInst(mContext, mMainFunction, arguments, "");

  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, createMallocTest) {
  Type* structType = Type::getInt8Ty(mLLVMContext);
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, Type::getInt32Ty(mLLVMContext));
  Instruction* instruction = IRWriter::createMalloc(mContext, structType, allocSize, "");

  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *instruction;
  ASSERT_STREQ(mStringStream->str().c_str(),
               "  %malloccall = tail call i8* @malloc(i32 ptrtoint "
               "(i8* getelementptr (i8, i8* null, i32 1) to i32))");
  
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
  
  IRWriter::createMalloc(mContext, structType, allocSize, "");
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, createFreeTest) {
  Value* value = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Instruction* instruction = IRWriter::createFree(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  tail call void @free(i8* null)");
  
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
  
  IRWriter::createFree(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, createGetElementPtrInstTest) {
  Value* value = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  vector<Value*> index;
  GetElementPtrInst* getElementPtrInst = IRWriter::createGetElementPtrInst(mContext, value, index);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *getElementPtrInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %0 = getelementptr i8, i8* null");
  
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
  
  IRWriter::createGetElementPtrInst(mContext, value, index);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, newBitCastInst) {
  PointerType* int8PointerType = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  PointerType* castToType = int8PointerType->getPointerTo();
  Value* value = ConstantPointerNull::get(int8PointerType);
  BitCastInst* bitCastInst = IRWriter::newBitCastInst(mContext, value, castToType);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *bitCastInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %0 = bitcast i8* null to i8**");
  
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
  
  IRWriter::newBitCastInst(mContext, value, castToType);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, newStoreInst) {
  PointerType* int32PointerType = Type::getInt32Ty(mLLVMContext)->getPointerTo();
  Value* pointer = ConstantPointerNull::get(int32PointerType);
  ConstantInt* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 0);
  StoreInst* storeInst = IRWriter::newStoreInst(mContext, value, pointer);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *storeInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  store i32 0, i32* null");
  
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
  
  IRWriter::newStoreInst(mContext, value, pointer);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, newAllocaInst) {
  Type* int32Type = Type::getInt32Ty(mLLVMContext);
  ConstantInt* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 0);
  AllocaInst* allocaInst = IRWriter::newAllocaInst(mContext, int32Type, "foo");
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *allocaInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
  
  IRWriter::newAllocaInst(mContext, int32Type, "foo");
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
}

TEST_F(IRWriterTest, newLoadInst) {
  PointerType* int32PointerType = Type::getInt32Ty(mLLVMContext)->getPointerTo();
  Value* pointer = ConstantPointerNull::get(int32PointerType);
  LoadInst* loadInst = IRWriter::newLoadInst(mContext, pointer, "foo");
  ConstantInt* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 0);
  
  EXPECT_EQ(mBasicBlock->size(), 1u);
  *mStringStream << *loadInst;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %foo = load i32, i32* null");
  
  IRWriter::createReturnInst(mContext, value);
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
  
  IRWriter::newLoadInst(mContext, pointer, "foo");
  
  EXPECT_EQ(mBasicBlock->size(), 2u);
}
