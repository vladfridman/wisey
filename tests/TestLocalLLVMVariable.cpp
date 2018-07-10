//
//  TestLocalLLVMVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalLLVMVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalLLVMVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalLLVMVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LocalLLVMVariableTest() {
    delete mStringStream;
  }
};

TEST_F(LocalLLVMVariableTest, basicFieldsTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");
  LocalLLVMVariable variable("foo", LLVMPrimitiveTypes::I16, alloc, 0);
  
  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(LLVMPrimitiveTypes::I16, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isStatic());
}

TEST_F(LocalLLVMVariableTest, generateAssignmentIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt16Ty(mLLVMContext), "foo");
  
  LocalLLVMVariable variable("foo", LLVMPrimitiveTypes::I16, alloc, 0);
  Value* assignValue = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 5);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(LLVMPrimitiveTypes::I16));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(assignValue));
  vector<const IExpression*> arrayIndices;
  
  variable.generateAssignmentIR(mContext, &expression, arrayIndices, 0);
  
  ASSERT_EQ(1ul, mDeclareBlock->size());
  *mStringStream << mDeclareBlock->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i16");
  mStringBuffer.clear();
  
  ASSERT_EQ(1ul, mEntryBlock->size());
  *mStringStream << mEntryBlock->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i16 5, i16* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalLLVMVariableTest, generateIdentifierIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt16Ty(mLLVMContext), "foo");
  LocalLLVMVariable variable("foo", LLVMPrimitiveTypes::I16, alloc, 0);
  
  variable.generateIdentifierIR(mContext, 0);
  
  ASSERT_EQ(1ul, mDeclareBlock->size());
  *mStringStream << mDeclareBlock->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i16");
  mStringBuffer.clear();
  
  ASSERT_EQ(1ul, mEntryBlock->size());
  *mStringStream << mEntryBlock->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  %0 = load i16, i16* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalLLVMVariableTest, generateIdentifierReferenceIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt16Ty(mLLVMContext), "foo");
  LocalLLVMVariable variable("foo", LLVMPrimitiveTypes::I16, alloc, 0);
  
  EXPECT_EQ(alloc, variable.generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(TestFileRunner, localLLVMVariableRunTest) {
  runFile("tests/samples/test_local_llvm_variable.yz", 5);
}
