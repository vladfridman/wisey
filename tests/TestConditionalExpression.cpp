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
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ConditionalExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Model.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
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
  NiceMock<MockVariable>* mVariable;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  ConditionalExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mConditionExpression(new NiceMock<MockExpression>()),
  mIfTrueExpression(new NiceMock<MockExpression>()),
  mIfFalseExpression(new NiceMock<MockExpression>()),
  mVariable(new NiceMock<MockVariable>()) {
    TestPrefix::generateIR(mContext);
    
    LLVMContext &llvmContext = mContext.getLLVMContext();
    
    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);

    Value* ifTrueValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    Value* ifFalseValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ON_CALL(*mIfTrueExpression, generateIR(_, _)).WillByDefault(Return(ifTrueValue));
    ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mIfFalseExpression, generateIR(_, _)).WillByDefault(Return(ifFalseValue));
    ON_CALL(*mIfFalseExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mConditionExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
    
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();

    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateShortNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ConditionalExpressionTest() {
    delete mFunction;
    delete mStringStream;
  }
  
  static void printConditionalExpression(IRGenerationContext& context, iostream& stream) {
    stream << "a";
  }
  
  static void printIfTrueExpression(IRGenerationContext& context, iostream& stream) {
    stream << "b";
  }
  
  static void printIfFalseExpression(IRGenerationContext& context, iostream& stream) {
    stream << "c";
  }
};

TEST_F(ConditionalExpressionTest, getVariableTest) {
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  EXPECT_EQ(expression.getVariable(mContext), nullptr);
}

TEST_F(ConditionalExpressionTest, conditionalExpressionRunWithFalse) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 0);
  ON_CALL(*mConditionExpression, generateIR(_, _)).WillByDefault(testing::Return(conditionValue));

  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  expression.generateIR(mContext, IR_GENERATION_NORMAL);

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
  ON_CALL(*mConditionExpression, generateIR(_, _)).WillByDefault(testing::Return(conditionValue));
 
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  expression.generateIR(mContext, IR_GENERATION_NORMAL);
  
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

TEST_F(ConditionalExpressionTest, isConstantTest) {
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);

  EXPECT_FALSE(expression.isConstant());
}

TEST_F(ConditionalExpressionTest, printToStreamTest) {
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);

  stringstream stringStream;
  ON_CALL(*mConditionExpression, printToStream(_, _))
  .WillByDefault(Invoke(printConditionalExpression));
  ON_CALL(*mIfTrueExpression, printToStream(_, _)).WillByDefault(Invoke(printIfTrueExpression));
  ON_CALL(*mIfFalseExpression, printToStream(_, _)).WillByDefault(Invoke(printIfFalseExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("a ? b : c", stringStream.str().c_str());
}

TEST_F(ConditionalExpressionTest, incompatibleTypesDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  Mock::AllowLeak(mVariable);
  
  Value* trueValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.5);
  ON_CALL(*mIfTrueExpression, generateIR(_, _)).WillByDefault(Return(trueValue));
  ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types in conditional expression operation");
}

TEST_F(ConditionalExpressionTest, voidTypesDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  Mock::AllowLeak(mVariable);
  
  ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  ON_CALL(*mIfFalseExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Error: Can not use expressions of type VOID in a conditional expression");
}

TEST_F(ConditionalExpressionTest, conditionIsNotBooleanDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  Mock::AllowLeak(mVariable);
 
  ON_CALL(*mConditionExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Condition in a conditional expression is not of type BOOLEAN");
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

TEST_F(TestFileSampleRunner, conditionalExpressionAddReferenceToOwnerRunTest) {
  runFile("tests/samples/test_conditional_expression_add_reference_to_owner.yz", "3");
}
