//
//  TestThrowStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThrowStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "TestFileSampleRunner.hpp"
#include "yazyk/Block.hpp"
#include "yazyk/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

extern Block* programBlock;

struct ThrowStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression> mMockExpression;
  NiceMock<MockType> mMockType;
  Model* mCircleModel;
  BasicBlock* mBlock;
  
  ThrowStatementTest() : mLLVMContext(mContext.getLLVMContext()) {
    StructType* circleStructType = StructType::create(mLLVMContext, "MCircle");
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    vector<Method*> circleMethods;
    map<string, Field*> circleFields;
    vector<Interface*> circleInterfaces;
    mCircleModel = new Model("MCircle",
                             circleStructType,
                             circleFields,
                             circleMethods,
                             circleInterfaces);
    Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, "model.MCircle.name");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       "model.MCircle.name");

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
}
};

TEST_F(ThrowStatementTest, wrongExpressionTypeDeathTest) {
  Mock::AllowLeak(&mMockType);
  Mock::AllowLeak(&mMockExpression);
  
  ON_CALL(mMockType, getTypeKind()).WillByDefault(Return(CONTROLLER_TYPE));
  ON_CALL(mMockExpression, getType(_)).WillByDefault(Return(&mMockType));
  ThrowStatement throwStatement(mMockExpression);
  
  EXPECT_EXIT(throwStatement.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Thrown object can only be a model");
}

TEST_F(ThrowStatementTest, modelExpressionTypeTest) {
  Constant* exceptionObject =
    ConstantPointerNull::get((PointerType*) mCircleModel->getLLVMType(mLLVMContext));
  ON_CALL(mMockExpression, getType(_)).WillByDefault(Return(mCircleModel));
  ON_CALL(mMockExpression, generateIR(_)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression);
  
  Value* result = throwStatement.generateIR(mContext);
  EXPECT_NE(result, nullptr);
}

TEST_F(TestFileSampleRunner, throwStatementRunDeathTest) {
  expectDeathDuringRun("tests/samples/test_throw.yz",
                       "libc\\+\\+abi\\.dylib: terminating with uncaught "
                       "exception of type MExcpeption");
}
