//
//  TestParameterLLVMVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterLLVMVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ParameterLLVMVariable.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterLLVMVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterLLVMVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterLLVMVariableTest, basicFieldsTest) {
  Value* fooValueStore = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterLLVMVariable variable("foo", LLVMPrimitiveTypes::I32, fooValueStore);
  
  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(LLVMPrimitiveTypes::I32, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(ParameterLLVMVariableTest, generateIdentifierIRTest) {
  Value* fooValueStore = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterLLVMVariable variable("foo", LLVMPrimitiveTypes::I32, fooValueStore);
  
  EXPECT_EQ(fooValueStore, variable.generateIdentifierIR(mContext));
}

TEST_F(ParameterLLVMVariableTest, parameterReferenceVariableAssignmentDeathTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterLLVMVariable variable("foo", LLVMPrimitiveTypes::I32, fooValue);
  vector<const IExpression*> arrayIndices;
  
  EXPECT_EXIT(variable.generateAssignmentIR(mContext, NULL, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(ParameterLLVMVariableTest, parameterReferenceVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterLLVMVariable variable("foo", LLVMPrimitiveTypes::I32, fooValue);
  
  EXPECT_EQ(variable.generateIdentifierIR(mContext), fooValue);
}

TEST_F(TestFileRunner, parameterLLVMVariableRunTest) {
  runFile("tests/samples/test_parameter_llvm_variable.yz", "3");
}
