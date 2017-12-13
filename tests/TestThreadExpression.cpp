//
//  TestThreadExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThreadExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockVariable.hpp"
#include "TestPrefix.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Names.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ThreadExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mThreadController;
  LocalReferenceVariable* mThreadVariable;
  ThreadExpression mThreadExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  ThreadExpressionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mThreadController = mContext.getController(Names::getThreadControllerFullName());
    PointerType* controllerType = mThreadController->getLLVMType(mContext);
    Value* threadStore = IRWriter::newAllocaInst(mContext, controllerType, "threadStore");
    llvm::Constant* null = ConstantPointerNull::get(controllerType);
    IRWriter::newStoreInst(mContext, null, threadStore);
    mThreadVariable = new LocalReferenceVariable(ThreadExpression::THREAD,
                                                 mThreadController,
                                                 threadStore);
    FakeExpression* fakeExpression = new FakeExpression(null, mThreadController);
    mThreadVariable->generateAssignmentIR(mContext, fakeExpression, 0);
    
    mContext.getScopes().setVariable(mThreadVariable);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ThreadExpressionTest() { }
};

TEST_F(ThreadExpressionTest, getVariableTest) {
  EXPECT_EQ(mThreadExpression.getVariable(mContext), mThreadVariable);
}

TEST_F(ThreadExpressionTest, getTypeTest) {
  EXPECT_EQ(mThreadExpression.getType(mContext), mThreadController);
}

TEST_F(ThreadExpressionTest, generateIRTest) {
  Value* instruction = mThreadExpression.generateIR(mContext, IR_GENERATION_NORMAL);

  *mStringStream << *instruction;
  string expected = "  %3 = load %wisey.lang.CThread*, %wisey.lang.CThread** %threadStore";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThreadExpressionTest, isConstantTest) {
  EXPECT_FALSE(mThreadExpression.isConstant());
}

TEST_F(ThreadExpressionTest, printToStreamTest) {
  stringstream stringStream;
  mThreadExpression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("thread", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, threadExpressionTypeRunTest) {
  runFile("tests/samples/test_thread_expression_type.yz", "1");
}

TEST_F(TestFileSampleRunner, threadExpressionNotNullRunTest) {
  runFile("tests/samples/test_thread_expression_not_null.yz", "1");
}
