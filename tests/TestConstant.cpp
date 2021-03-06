//
//  TestConstant.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Constant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "Constant.hpp"
#include "PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ConstantTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mExpression;
  NiceMock<MockObjectType>* mObject;
  string mName;
  Constant* mConstant;
  Constant* mExternalConstant;

public:
  
  ConstantTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()),
  mObject(new NiceMock<MockObjectType>()),
  mName("MYCONSTANT") {
    TestPrefix::generateIR(mContext);
    
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    ON_CALL(*mExpression, isConstant()).WillByDefault(Return(true));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    llvm::Constant* constantInt = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(constantInt));
    ON_CALL(*mObject, getTypeName()).WillByDefault(Return("MObject"));

    mConstant = new Constant(PUBLIC_ACCESS, PrimitiveTypes::INT, mName, mExpression, 3);
    mExternalConstant = new Constant(PUBLIC_ACCESS, PrimitiveTypes::INT, mName, NULL, 3);
  }
  
  ~ConstantTest() {
    delete mExpression;
    delete mObject;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(ConstantTest, gettersTest) {
  EXPECT_TRUE(mConstant->isPublic());
  EXPECT_EQ(mConstant->getName(), mName);
  EXPECT_EQ(mConstant->getType(), PrimitiveTypes::INT);
}

TEST_F(ConstantTest, elementTypeTest) {
  EXPECT_TRUE(mConstant->isConstant());
  EXPECT_FALSE(mConstant->isField());
  EXPECT_FALSE(mConstant->isMethod());
  EXPECT_FALSE(mConstant->isStaticMethod());
  EXPECT_FALSE(mConstant->isMethodSignature());
  EXPECT_FALSE(mConstant->isLLVMFunction());
}

TEST_F(ConstantTest, getConstantGlobalNameTest) {
  EXPECT_STREQ(mConstant->getConstantGlobalName(mObject).c_str(), "constant.MObject.MYCONSTANT");
}

TEST_F(ConstantTest, defineTest) {
  EXPECT_EQ(mContext.getModule()->getNamedGlobal("constant.MObject.MYCONSTANT"), nullptr);
  
  mConstant->define(mContext, mObject);
  
  EXPECT_NE(mContext.getModule()->getNamedGlobal("constant.MObject.MYCONSTANT"), nullptr);
}

TEST_F(ConstantTest, defineForNonConstantExpressionDeathTest) {
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(mObject);
  ON_CALL(*mExpression, isConstant()).WillByDefault(Return(false));

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mConstant->define(mContext, mObject));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Constant is initialized with a non-constant expression\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ConstantTest, typeMismatchDeathTest) {
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(mObject);
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR));

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mConstant->define(mContext, mObject));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Constant value type does not match declared constant type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ConstantTest, printPublicConstantToStreamTest) {
  stringstream stringStream;
  mConstant->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  constant int MYCONSTANT = expression;\n", stringStream.str().c_str());
}

TEST_F(ConstantTest, printPrivateConstantToStreamTest) {
  stringstream stringStream;
  Constant* constant = new Constant(PRIVATE_ACCESS, PrimitiveTypes::INT, mName, mExpression, 0);
  constant->printToStream(mContext, stringStream);

  EXPECT_EQ(stringStream.str().size(), 0u);
}

TEST_F(TestFileRunner, constantInitializedWithNonConstantDeathRunTest) {
  expectFailCompile("tests/samples/test_constant_initialized_with_non_constant.yz",
                    1,
                    "tests/samples/test_constant_initialized_with_non_constant.yz\\(4\\): Error: Constant is initialized with a non-constant expression");
}
