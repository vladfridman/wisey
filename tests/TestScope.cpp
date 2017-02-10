//
//  TestScope.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Scope}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/Scope.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

using ::testing::Test;

struct ScopeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Function* mFunction;
  Scope mScope;
 
public:
  
  ScopeTest() : mLLVMContext(mContext.getLLVMContext()), mScope(Scope()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
  }
};

TEST_F(ScopeTest, LocalsTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  mScope.getLocals()["foo"] = new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  
  EXPECT_EQ(mScope.getLocals().count("foo"), 1ul);
  EXPECT_EQ(mScope.getLocals()["foo"]->getValue(), fooValue);
  EXPECT_EQ(mScope.getLocals().count("bar"), 0ul);
}

TEST_F(ScopeTest, MaybeFreeOwnedMemoryHeapVariableTest) {
  Value* fooValue = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
  mScope.getLocals()["foo"] = new LocalHeapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  
  EXPECT_EQ(mBlock->getInstList().size(), 0ul);

  mScope.maybeFreeOwnedMemory(mBlock);
  
  EXPECT_GT(mBlock->getInstList().size(), 0ul);
}

TEST_F(ScopeTest, MaybeFreeOwnedMemoryStackVariableTest) {
  Value* fooValue = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
  mScope.getLocals()["foo"] = new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  
  EXPECT_EQ(mBlock->getInstList().size(), 0ul);
  
  mScope.maybeFreeOwnedMemory(mBlock);
  
  EXPECT_EQ(mBlock->getInstList().size(), 0ul);
}
