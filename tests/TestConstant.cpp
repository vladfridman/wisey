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
#include "wisey/Constant.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
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
  
public:
  
  ConstantTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()),
  mObject(new NiceMock<MockObjectType>()),
  mName("MYCONSTANT") {
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    ON_CALL(*mExpression, isConstant()).WillByDefault(Return(true));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    llvm::Constant* constantInt = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mExpression, generateIR(_)).WillByDefault(Return(constantInt));
    ON_CALL(*mObject, getName()).WillByDefault(Return("MObject"));

    mConstant = new Constant(PUBLIC_ACCESS, PrimitiveTypes::INT_TYPE, mName, mExpression);
  }
  
  ~ConstantTest() {
    delete mExpression;
    delete mObject;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(ConstantTest, getObjectElementTypeTest) {
  EXPECT_EQ(mConstant->getObjectElementType(), OBJECT_ELEMENT_CONSTANT);
}

TEST_F(ConstantTest, getConstantGlobalNameTest) {
  EXPECT_STREQ(mConstant->getConstantGlobalName(mObject).c_str(), "constant.MObject.MYCONSTANT");
}

TEST_F(ConstantTest, generateIRTest) {
  EXPECT_EQ(mContext.getModule()->getNamedGlobal("constant.MObject.MYCONSTANT"), nullptr);

  mConstant->generateIR(mContext, mObject);
  
  EXPECT_NE(mContext.getModule()->getNamedGlobal("constant.MObject.MYCONSTANT"), nullptr);
}

TEST_F(ConstantTest, printPublicConstantToStreamTest) {
  stringstream stringStream;
  mConstant->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  public constant int MYCONSTANT = expression;\n", stringStream.str().c_str());
}

TEST_F(ConstantTest, printPrivateConstantToStreamTest) {
  stringstream stringStream;
  Constant* constant = new Constant(PRIVATE_ACCESS, PrimitiveTypes::INT_TYPE, mName, mExpression);
  constant->printToStream(mContext, stringStream);

  EXPECT_EQ(stringStream.str().size(), 0u);
}
