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
#include "TestFileRunner.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
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
  Interface* mThreadInterface;
  LocalReferenceVariable* mThreadVariable;
  ThreadExpression* mThreadExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  ThreadExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mThreadExpression(new ThreadExpression(0)) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mThreadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    llvm::PointerType* llvmType = mThreadInterface->getLLVMType(mContext);
    Value* threadStore = IRWriter::newAllocaInst(mContext, llvmType, "threadStore");
    llvm::Constant* null = ConstantPointerNull::get(llvmType);
    IRWriter::newStoreInst(mContext, null, threadStore);
    mThreadVariable = new LocalReferenceVariable(ThreadExpression::THREAD,
                                                 mThreadInterface,
                                                 threadStore);
    FakeExpression* fakeExpression = new FakeExpression(null, mThreadInterface);
    vector<const IExpression*> arrayIndices;
    mThreadVariable->generateAssignmentIR(mContext, fakeExpression, arrayIndices, 0);
    
    mContext.getScopes().setVariable(mThreadVariable);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ThreadExpressionTest() { }
};

TEST_F(ThreadExpressionTest, getVariableTest) {
  vector<const IExpression*> arrayIndices;
  EXPECT_EQ(mThreadExpression->getVariable(mContext, arrayIndices), mThreadVariable);
}

TEST_F(ThreadExpressionTest, getTypeTest) {
  EXPECT_EQ(mThreadExpression->getType(mContext), mThreadInterface);
}

TEST_F(ThreadExpressionTest, generateIRTest) {
  Value* instruction = mThreadExpression->generateIR(mContext, PrimitiveTypes::VOID_TYPE);

  *mStringStream << *instruction;
  string expected = "  %3 = load %wisey.lang.threads.IThread*, %wisey.lang.threads.IThread** %threadStore";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThreadExpressionTest, isConstantTest) {
  EXPECT_FALSE(mThreadExpression->isConstant());
}

TEST_F(ThreadExpressionTest, isAssignableTest) {
  EXPECT_TRUE(mThreadExpression->isAssignable());
}

TEST_F(ThreadExpressionTest, printToStreamTest) {
  stringstream stringStream;
  mThreadExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("thread", stringStream.str().c_str());
}

TEST_F(TestFileRunner, threadExpressionTypeRunTest) {
  runFile("tests/samples/test_thread_expression_type.yz", "1");
}

TEST_F(TestFileRunner, threadExpressionNotNullRunTest) {
  runFile("tests/samples/test_thread_expression_not_null.yz", "1");
}

TEST_F(TestFileRunner, threadMethodBothConcealedAndRevealedDeathTest) {
  expectFailCompile("tests/samples/test_thread_method_both_concealed_and_revealed.yz",
                    1,
                    "Error: Method 'exit' in object systems.vos.wisey.compiler.tests.CThread can "
                    "either be marked with a conceal or reveal qualifier but not both");
}

TEST_F(TestFileRunner, threadRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_run.yz",
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n",
                     "");
}

TEST_F(TestFileRunner, threadResetRunTest) {
  runFileCheckOutput("tests/samples/test_thread_reset.yz",
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n"
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n",
                     "");
}

TEST_F(TestFileRunner, threadProducerRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_producer_run.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n",
                     "");
}

TEST_F(TestFileRunner, threadProducerResetRunTest) {
  runFileCheckOutput("tests/samples/test_thread_producer_reset.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n"
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n",
                     "");
}

TEST_F(TestFileRunner, threadCancelRunTest) {
  runFileCheckOutput("tests/samples/test_thread_cancel.yz",
                     "Thread started = 0, has result = 0, was cancelled = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has result = 0, was cancelled = 0\n"
                     "Worker thread cancelled\n"
                     "Thread started = 1, has result = 0, was cancelled = 1\n"
                     "Thread result is null\n",
                     "");
}

TEST_F(TestFileRunner, threadThrowConcealedCallExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_thread_throw_concealed_call_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.threads.MThreadConcealedMethodException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_thread_throw_concealed_call_exception.yz:19)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, threadThrowRevealedCallExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_thread_throw_revealed_call_exception.yz",
                               0,
                               "",
                               "Unhandled exception wisey.lang.threads.MThreadRevealedMethodException\n"
                               "  at systems.vos.wisey.compiler.tests.MWorker.work(tests/samples/test_thread_throw_revealed_call_exception.yz:10)\n");
}
