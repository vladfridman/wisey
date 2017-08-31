//
//  TestConditionalExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ConditionalExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ConditionalExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Model.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ConditionalExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mConditionExpression;
  NiceMock<MockExpression>* mIfTrueExpression;
  NiceMock<MockExpression>* mIfFalseExpression;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  ConditionalExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mConditionExpression(new NiceMock<MockExpression>()),
  mIfTrueExpression(new NiceMock<MockExpression>()),
  mIfFalseExpression(new NiceMock<MockExpression>()) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    
    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    mModel = new Model(modelFullName, structType);

    Value* ifTrueValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    Value* ifFalseValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ON_CALL(*mIfTrueExpression, generateIR(_)).WillByDefault(Return(ifTrueValue));
    ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mIfFalseExpression, generateIR(_)).WillByDefault(Return(ifFalseValue));
    ON_CALL(*mIfFalseExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mConditionExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
    
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();

    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ConditionalExpressionTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(ConditionalExpressionTest, conditionalExpressionRunWithFalse) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 0);
  ON_CALL(*mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));

  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  expression.generateIR(mContext);

  ASSERT_EQ(4ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 false, label %cond.true, label %cond.false");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.true", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.false", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %cond = phi i32 [ 3, %cond.true ], [ 5, %cond.false ]");
}

TEST_F(ConditionalExpressionTest, conditionalExpressionRunWithTrue) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));
 
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(4ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %cond.true, label %cond.false");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.true", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.false", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %cond = phi i32 [ 3, %cond.true ], [ 5, %cond.false ]");
}

TEST_F(ConditionalExpressionTest, existsInOuterScopeTest) {
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);

  ON_CALL(*mIfTrueExpression, existsInOuterScope(_)).WillByDefault(Return(true));
  ON_CALL(*mIfFalseExpression, existsInOuterScope(_)).WillByDefault(Return(true));
  EXPECT_TRUE(expression.existsInOuterScope(mContext));
  
  ON_CALL(*mIfTrueExpression, existsInOuterScope(_)).WillByDefault(Return(false));
  ON_CALL(*mIfFalseExpression, existsInOuterScope(_)).WillByDefault(Return(true));
  EXPECT_FALSE(expression.existsInOuterScope(mContext));
  
  ON_CALL(*mIfTrueExpression, existsInOuterScope(_)).WillByDefault(Return(true));
  ON_CALL(*mIfFalseExpression, existsInOuterScope(_)).WillByDefault(Return(false));
  EXPECT_FALSE(expression.existsInOuterScope(mContext));
  
  ON_CALL(*mIfTrueExpression, existsInOuterScope(_)).WillByDefault(Return(false));
  ON_CALL(*mIfFalseExpression, existsInOuterScope(_)).WillByDefault(Return(false));
  EXPECT_FALSE(expression.existsInOuterScope(mContext));
}

TEST_F(ConditionalExpressionTest, releaseOwnershipTest) {
  Type* type = mModel->getOwner()->getLLVMType(mLLVMContext);
  Value* ifTrueValue = IRWriter::newAllocaInst(mContext, type, "ifTrueValue");
  Value* ifFalseValue = IRWriter::newAllocaInst(mContext, type, "ifFalseValue");
  ON_CALL(*mIfTrueExpression, generateIR(_)).WillByDefault(Return(ifTrueValue));
  ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  ON_CALL(*mIfFalseExpression, generateIR(_)).WillByDefault(Return(ifFalseValue));
  ON_CALL(*mIfFalseExpression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  expression.releaseOwnership(mContext);

  *mStringStream << *mFunction;
  string expected = string() +
  "\ndefine internal i32 @test() {"
  "\nentry:"
  "\n  %ifTrueValue = alloca %systems.vos.wisey.compiler.tests.MModel*"
  "\n  %ifFalseValue = alloca %systems.vos.wisey.compiler.tests.MModel*"
  "\n  br i1 true, label %cond.release.true, label %cond.release.false"
  "\n"
  "\ncond.release.true:                                ; preds = %entry"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MModel("
  "%systems.vos.wisey.compiler.tests.MModel** %ifFalseValue)"
  "\n  br label %cond.release.end"
  "\n"
  "\ncond.release.false:                               ; preds = %entry"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MModel("
  "%systems.vos.wisey.compiler.tests.MModel** %ifTrueValue)"
  "\n  br label %cond.release.end"
  "\n"
  "\ncond.release.end:                                 "
  "; preds = %cond.release.false, %cond.release.true"
  "\n}\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ConditionalExpressionTest, incompatibleTypesDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  Value* trueValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.5);
  ON_CALL(*mIfTrueExpression, generateIR(_)).WillByDefault(Return(trueValue));
  ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types in conditional expression operation");
}

TEST_F(ConditionalExpressionTest, voidTypesDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  ON_CALL(*mIfFalseExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not use expressions of type VOID in a conditional expression");
}

TEST_F(ConditionalExpressionTest, conditionIsNotBooleanDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  ON_CALL(*mConditionExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Condition in a conditional expression is not of type BOOLEAN");
}

TEST_F(ConditionalExpressionTest, addReferenceToOwnerDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);

  EXPECT_EXIT(expression.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to non owner type conditional expression");
}

TEST_F(TestFileSampleRunner, conditionalExpressionRunTrueConditionRunTest) {
  runFile("tests/samples/test_conditional_with_true.yz", "3");
}

TEST_F(TestFileSampleRunner, conditionalExpressionRunFlaseConditionRunTest) {
  runFile("tests/samples/test_conditional_with_false.yz", "5");
}

TEST_F(TestFileSampleRunner, conditionalExpressionReleaseOwnershipRunTest) {
  runFile("tests/samples/test_conditional_expression_release_ownership.yz", "1");
}
