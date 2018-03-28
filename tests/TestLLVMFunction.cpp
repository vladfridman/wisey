//
//  TestLLVMFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunction}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMFunctionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  LLVMFunction* mLLVMFunction;
  const LLVMFunctionType* mLLVMFunctionType;
  NiceMock<MockConcreteObjectType>* mObject = new NiceMock<MockConcreteObjectType>();
  llvm::Function* mFunctionInLLVM;

  LLVMFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
    LLVMFunctionArgument* llvmFunctionArgument =
    new LLVMFunctionArgument(LLVMPrimitiveTypes::I64, "input");
    vector<const LLVMFunctionArgument*> arguments;
    arguments.push_back(llvmFunctionArgument);
    Block* block = new Block();
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    vector<const IType*> argumentTypes;
    argumentTypes.push_back(LLVMPrimitiveTypes::I64);
    mLLVMFunctionType = new LLVMFunctionType(LLVMPrimitiveTypes::I16, argumentTypes);
    mLLVMFunction = new LLVMFunction("myfunction",
                                     mLLVMFunctionType,
                                     LLVMPrimitiveTypes::I16,
                                     arguments,
                                     compoundStatement,
                                     0);

    ON_CALL(*mObject, getTypeName()).WillByDefault(Return("MSomeObject"));

    vector<Type*> argumentLLVMTypes;
    for (const IType* argumentType : argumentTypes) {
      argumentLLVMTypes.push_back(argumentType->getLLVMType(mContext));
    }
    Type* llvmReturnType = LLVMPrimitiveTypes::I16->getLLVMType(mContext);
    FunctionType* ftype = FunctionType::get(llvmReturnType, argumentLLVMTypes, false);
    string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(mObject,
                                                                               "myfunction");
    mFunctionInLLVM = Function::Create(ftype,
                                       GlobalValue::InternalLinkage,
                                       functionName,
                                       mContext.getModule());
  }
  
  ~LLVMFunctionTest() {
    delete mObject;
  }
};

TEST_F(LLVMFunctionTest, objectElementTypeTest) {
  EXPECT_FALSE(mLLVMFunction->isConstant());
  EXPECT_FALSE(mLLVMFunction->isField());
  EXPECT_FALSE(mLLVMFunction->isMethod());
  EXPECT_FALSE(mLLVMFunction->isStaticMethod());
  EXPECT_FALSE(mLLVMFunction->isMethodSignature());
  EXPECT_TRUE(mLLVMFunction->isLLVMFunction());
}

TEST_F(LLVMFunctionTest, getNameTest) {
  EXPECT_STREQ("myfunction", mLLVMFunction->getName().c_str());
}

TEST_F(LLVMFunctionTest, getTypeTest) {
  EXPECT_EQ(mLLVMFunctionType, mLLVMFunction->getType());
}

TEST_F(LLVMFunctionTest, getLLVMFunctionTest) {
  EXPECT_EQ(mFunctionInLLVM, mLLVMFunction->getLLVMFunction(mContext, mObject));
}
