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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ParameterPointerVariable.hpp"

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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mPointerType = LLVMPrimitiveTypes::I64->getPointerType();
    mVariableValue = ConstantPointerNull::get(mPointerType->getLLVMType(mContext));
    mVariable = new ParameterPointerVariable("foo", mPointerType, mVariableValue);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterPointerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mPointerType, mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isSystem());
}

TEST_F(ParameterPointerVariableTest, assignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  
  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(ParameterPointerVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mVariableValue, mVariable->generateIdentifierIR(mContext));
}

TEST_F(TestFileRunner, parameterPointerVariableRunTest) {
  runFile("tests/samples/test_parameter_pointer_variable.yz", "3");
}
