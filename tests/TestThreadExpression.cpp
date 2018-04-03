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
  Interface* mThreadInterface;
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

    mThreadInterface = mContext.getInterface(Names::getThreadInterfaceFullName());
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
  EXPECT_EQ(mThreadExpression.getVariable(mContext, arrayIndices), mThreadVariable);
}

TEST_F(ThreadExpressionTest, getTypeTest) {
  EXPECT_EQ(mThreadExpression.getType(mContext), mThreadInterface);
}

TEST_F(ThreadExpressionTest, generateIRTest) {
  Value* instruction = mThreadExpression.generateIR(mContext, PrimitiveTypes::VOID_TYPE);

  *mStringStream << *instruction;
  string expected = "  %3 = load %wisey.lang.IThread*, %wisey.lang.IThread** %threadStore";
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

TEST_F(TestFileRunner, threadExpressionTypeRunTest) {
  runFile("tests/samples/test_thread_expression_type.yz", "1");
}

TEST_F(TestFileRunner, threadExpressionNotNullRunTest) {
  runFile("tests/samples/test_thread_expression_not_null.yz", "1");
}
