//
//  TestCastExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CastExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/CastExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
  MOCK_CONST_METHOD1(releaseOwnership, void (IRGenerationContext&));
};

class MockTypeSpecifier : public ITypeSpecifier {
public:
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
};

struct CastExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockExpression> mExpression;
  NiceMock<MockTypeSpecifier> mTypeSpecifier;
  Interface* mCarInterface;
  
public:
  
  CastExpressionTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> carInterfaceTypes;
    StructType* carInterfaceStructType = StructType::create(mContext.getLLVMContext(), "ICar");
    carInterfaceStructType->setBody(carInterfaceTypes);
    vector<MethodSignature*> carInterfaceMethods;
    vector<Interface*> carParentInterfaces;
    mCarInterface = new Interface("ICar",
                                  carInterfaceStructType,
                                  carParentInterfaces,
                                  carInterfaceMethods);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~CastExpressionTest() {
    delete mStringStream;
  }
};

TEST_F(CastExpressionTest, CastExpressionAutoCastTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(expressionValue));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  ON_CALL(mTypeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));

  CastExpression castExpression(mTypeSpecifier, mExpression);
  
  result = castExpression.generateIR(mContext);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i1 true to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastExpressionTest, releaseOwnershipTest) {
  ON_CALL(mTypeSpecifier, getType(_)).WillByDefault(Return(mCarInterface));
  CastExpression castExpression(mTypeSpecifier, mExpression);
  
  EXPECT_CALL(mExpression, releaseOwnership(_)).Times(1);
  
  castExpression.releaseOwnership(mContext);
}

TEST_F(CastExpressionTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&mTypeSpecifier);
  ON_CALL(mTypeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  CastExpression castExpression(mTypeSpecifier, mExpression);
  
  EXPECT_EXIT(castExpression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a cast to primitive type, "
              "it is not a heap pointer");
}

TEST_F(TestFileSampleRunner, CastOrExpressionGrammarTest) {
  runFile("tests/samples/test_cast_or_expression.yz", "10");
}