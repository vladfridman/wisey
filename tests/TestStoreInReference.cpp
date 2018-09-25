//
//  TestStoreInReference.cpp
//  runtests
//
//  Created by Vladimir Fridman on 6/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StoreInReference}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instruction.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "StoreInReference.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMPrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct StoreInReferenceTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mValueExpression;
  NiceMock<MockExpression>* mPointerExpression;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  StoreInReference* mStoreInReference;
  
  StoreInReferenceTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mValueExpression(new NiceMock<MockExpression>()),
  mPointerExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    Value* value = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5);
    ON_CALL(*mValueExpression, generateIR(_, _)).WillByDefault(Return(value));
    ON_CALL(*mValueExpression, getType(_)).WillByDefault(Return(LLVMPrimitiveTypes::I64));

    const LLVMPointerType* pointerType = LLVMPrimitiveTypes::I64->getPointerType(mContext, 0);
    ON_CALL(*mPointerExpression, getType(_)).WillByDefault(Return(pointerType));
    value = ConstantPointerNull::get(pointerType->getLLVMType(mContext));
    ON_CALL(*mPointerExpression, generateIR(_, _)).WillByDefault(Return(value));

    mStoreInReference = new StoreInReference(mValueExpression, mPointerExpression, 5);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StoreInReferenceTest() {
    delete mStoreInReference;
    delete mStringStream;
  }
};

TEST_F(StoreInReferenceTest, generateIRTest) {
  mStoreInReference->generateIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  store i64 5, i64* null\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(StoreInReferenceTest, incorrectPointerTypeDeathTest) {
  ::Mock::AllowLeak(mValueExpression);
  ::Mock::AllowLeak(mPointerExpression);

  ON_CALL(*mPointerExpression, getType(_)).WillByDefault(Return(LLVMPrimitiveTypes::I64));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mStoreInReference->generateIR(mContext));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Second parameter in ::llvm::store is not of pointer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(StoreInReferenceTest, incorrectValueTypeDeathTest) {
  ::Mock::AllowLeak(mValueExpression);
  ::Mock::AllowLeak(mPointerExpression);
  
  ON_CALL(*mValueExpression, getType(_)).WillByDefault(Return(LLVMPrimitiveTypes::I8));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mStoreInReference->generateIR(mContext));
  EXPECT_STREQ("/tmp/source.yz(5): Error: First parameter in ::llvm::store is not of type that is compatable with pointer type of the second parameter\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
