//
//  TestParameterPointerOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterPointerOwnerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ParameterPointerOwnerVariable.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterPointerOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  ParameterPointerOwnerVariable* mVariable;
  Value* mVariableValue;
  
public:
  
  ParameterPointerOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    const LLVMPointerOwnerType* type = LLVMPrimitiveTypes::I8->getPointerType()->getOwner();
    mVariableValue = ConstantPointerNull::get(type->getLLVMType(mContext));
    mVariable = new ParameterPointerOwnerVariable("foo", type, mVariableValue);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterPointerOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(LLVMPrimitiveTypes::I8->getPointerType()->getOwner(), mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isSystem());
}

TEST_F(ParameterPointerOwnerVariableTest, assignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  
  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to function parameters is not allowed");
}

TEST_F(ParameterPointerOwnerVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mVariableValue, mVariable->generateIdentifierIR(mContext));
}
