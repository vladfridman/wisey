//
//  TestObjectFunctionVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectFunctionVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ObjectFunctionVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ObjectFunctionVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  ObjectFunctionVariable* mObjectFunctionVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockConcreteObjectType>* mObject = new NiceMock<MockConcreteObjectType>();
  const LLVMFunctionType* mLLVMFunctionType;
  LLVMFunction* mLLVMFunction;
  llvm::Function* mFunctionInLLVM;
  
  ObjectFunctionVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    const IType* returnType = LLVMPrimitiveTypes::I8->getPointerType();
    const IType* argumentType = LLVMPrimitiveTypes::I64;
    vector<const IType*> argumentTypes;
    argumentTypes.push_back(argumentType);
    mLLVMFunctionType = new LLVMFunctionType(returnType, argumentTypes);
    LLVMFunctionArgument* functionArgument = new LLVMFunctionArgument(argumentType, "input");
    vector<const LLVMFunctionArgument*> functionArguments;
    functionArguments.push_back(functionArgument);
    Block* block = new Block();
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    mLLVMFunction = new LLVMFunction("myfunction",
                                     PUBLIC_ACCESS,
                                     mLLVMFunctionType,
                                     returnType,
                                     functionArguments,
                                     compoundStatement,
                                     0);
    ON_CALL(*mObject, findLLVMFunction(_)).WillByDefault(Return(mLLVMFunction));
    ON_CALL(*mObject, getTypeName()).WillByDefault(Return("MSomeObject"));

    vector<Type*> argumentLLVMTypes;
    for (const LLVMFunctionArgument* argument : functionArguments) {
      argumentLLVMTypes.push_back(argument->getType()->getLLVMType(mContext));
    }
    Type* llvmReturnType = returnType->getLLVMType(mContext);
    FunctionType* ftype = FunctionType::get(llvmReturnType, argumentLLVMTypes, false);
    string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(mObject,
                                                                               "myfunction");
    mFunctionInLLVM = Function::Create(ftype,
                                       GlobalValue::InternalLinkage,
                                       functionName,
                                       mContext.getModule());

    mObjectFunctionVariable = new ObjectFunctionVariable("myfunction", mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ObjectFunctionVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(ObjectFunctionVariableTest, basicFieldsTest) {
  EXPECT_STREQ("myfunction", mObjectFunctionVariable->getName().c_str());
  EXPECT_EQ(mLLVMFunctionType, mObjectFunctionVariable->getType());
  EXPECT_FALSE(mObjectFunctionVariable->isField());
  EXPECT_FALSE(mObjectFunctionVariable->isSystem());
}

TEST_F(ObjectFunctionVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mFunctionInLLVM, mObjectFunctionVariable->generateIdentifierIR(mContext));
}

TEST_F(ObjectFunctionVariableTest, generateIdentifierIRDeathTest) {
  ::Mock::AllowLeak(mObject);
  
  ON_CALL(*mObject, findLLVMFunction(_)).WillByDefault(Return(nullptr));

  EXPECT_EXIT(mObjectFunctionVariable->generateIdentifierIR(mContext),
              ::testing::ExitedWithCode(1),
              "LLVM function myfunction is not found in object MSomeObject");
}
