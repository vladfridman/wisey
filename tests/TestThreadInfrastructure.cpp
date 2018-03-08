//
//  TestThreadInfrastructure.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThreadInfrastructure}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRWriter.hpp"
#include "wisey/ThreadInfrastructure.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ThreadInfrastructureTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Module* mModule;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Thread* mThread;
  
public:
  
  ThreadInfrastructureTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mModule(mContext.getModule()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mThread = Thread::newThread(PUBLIC_ACCESS, "TWorker", NULL);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ThreadInfrastructureTest, getThreadCreateFunctionTest) {
  EXPECT_EQ(mModule->getFunction("pthread_create"), nullptr);
  EXPECT_NE(ThreadInfrastructure::getThreadCreateFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("pthread_create"), nullptr);
}

TEST_F(ThreadInfrastructureTest, getRunBridgeFunctionTest) {
  EXPECT_EQ(mModule->getFunction("TWorker.runBridge"), nullptr);
  EXPECT_NE(ThreadInfrastructure::getRunBridgeFunction(mContext, mThread), nullptr);
  EXPECT_NE(mModule->getFunction("TWorker.runBridge"), nullptr);
}

TEST_F(ThreadInfrastructureTest, createNativeThreadTypeTest) {
  EXPECT_EQ(mModule->getTypeByName("struct._opaque_pthread_t"), nullptr);
  EXPECT_NE(ThreadInfrastructure::createNativeThreadType(mContext), nullptr);
  EXPECT_NE(mModule->getTypeByName("struct._opaque_pthread_t"), nullptr);
}

TEST_F(ThreadInfrastructureTest, createNativeThreadAttributesType) {
  EXPECT_EQ(mModule->getTypeByName("struct._opaque_pthread_attr_t"), nullptr);
  EXPECT_NE(ThreadInfrastructure::createNativeThreadAttributesType(mContext), nullptr);
  EXPECT_NE(mModule->getTypeByName("struct._opaque_pthread_attr_t"), nullptr);
}
