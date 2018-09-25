//
//  TestParameterPointerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterPointerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "ParameterPointerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterPointerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  ParameterPointerVariable* mVariable;
  Value* mVariableValue;
  const LLVMPointerType* mPointerType;
  
public:
  
  ParameterPointerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mPointerType = LLVMPrimitiveTypes::I64->getPointerType(mContext, 0);
    mVariableValue = ConstantPointerNull::get(mPointerType->getLLVMType(mContext));
    mVariable = new ParameterPointerVariable("foo", mPointerType, mVariableValue, 0);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterPointerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mPointerType, mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isStatic());
}

TEST_F(ParameterPointerVariableTest, assignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mVariable->generateAssignmentIR(mContext, NULL, arrayIndices, 9));
  EXPECT_STREQ("/tmp/source.yz(9): Error: Assignment to method parameters is not allowed\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ParameterPointerVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mVariableValue, mVariable->generateIdentifierIR(mContext, 0));
}

TEST_F(TestFileRunner, parameterPointerVariableRunTest) {
  runFile("tests/samples/test_parameter_pointer_variable.yz", 3);
}
