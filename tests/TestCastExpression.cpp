//
//  TestCastExpression.cpp
//  Wisey
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

#include "MockExpression.hpp"
#include "MockTypeSpecifier.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/CastExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct CastExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockExpression>* mExpression;
  NiceMock<MockTypeSpecifier>* mTypeSpecifier;
  Interface* mCarInterface;
  
public:
  
  CastExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()),
  mTypeSpecifier(new NiceMock<MockTypeSpecifier>()) {
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
    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mContext.getLLVMContext(), carFullName);
    carInterfaceStructType->setBody(carInterfaceTypes);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mCarInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                            carFullName,
                                            carInterfaceStructType,
                                            parentInterfaces,
                                            interfaceElements);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~CastExpressionTest() {
    delete mStringStream;
  }
  
  static void printTypeSpecifier(IRGenerationContext& context, iostream& stream) {
    stream << "int";
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "true";
  }
};

TEST_F(CastExpressionTest, isConstantTest) {
  CastExpression castExpression(mTypeSpecifier, mExpression, 0);

  EXPECT_FALSE(castExpression.isConstant());
}

TEST_F(CastExpressionTest, getVariableTest) {
  CastExpression castExpression(mTypeSpecifier, mExpression, 0);
  EXPECT_EQ(castExpression.getVariable(mContext), nullptr);
}

TEST_F(CastExpressionTest, castExpressionAutoCastTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(expressionValue));
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  ON_CALL(*mTypeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  EXPECT_CALL(*mExpression, generateIR(_, IR_GENERATION_NORMAL));

  CastExpression castExpression(mTypeSpecifier, mExpression, 0);
  
  result = castExpression.generateIR(mContext, IR_GENERATION_NORMAL);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i1 true to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastExpressionTest, generateIRWithReleaseTest) {
  ON_CALL(*mTypeSpecifier, getType(_)).WillByDefault(Return(mCarInterface));
  CastExpression castExpression(mTypeSpecifier, mExpression, 0);
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mCarInterface->getOwner()));

  EXPECT_CALL(*mExpression, generateIR(_, IR_GENERATION_RELEASE));
  
  castExpression.generateIR(mContext, IR_GENERATION_RELEASE);
}

TEST_F(CastExpressionTest, printToStreamTest) {
  CastExpression castExpression(mTypeSpecifier, mExpression, 0);
  ON_CALL(*mTypeSpecifier, printToStream(_, _)).WillByDefault(Invoke(printTypeSpecifier));
  ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));

  stringstream stringStream;
  castExpression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("(int) true", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, CastOrExpressionGrammarRunTest) {
  runFile("tests/samples/test_cast_or_expression.yz", "10");
}
