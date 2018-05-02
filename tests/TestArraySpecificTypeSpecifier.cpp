//
//  TestArraySpecificTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArraySpecificTypeSpecifier}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "MockExpression.hpp"
#include "wisey/ArraySpecificTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArraySpecificTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  list<const IExpression*> mDimensions;
  NiceMock<MockExpression> mThreeExpression;

  ArraySpecificTypeSpecifierTest() : mLLVMContext(mContext.getLLVMContext()) {
    llvm::Constant* three = llvm::ConstantInt::get(llvm::Type::getInt64Ty(mLLVMContext), 3);
    ON_CALL(mThreeExpression, generateIR(_, _)).WillByDefault(Return(three));
    ON_CALL(mThreeExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(mThreeExpression, printToStream(_, _)).WillByDefault(Invoke(printThree));
    mDimensions.push_back(&mThreeExpression);
  }
  
  static void printThree(IRGenerationContext& context, iostream& stream) {
    stream << "3";
  }
};

TEST_F(ArraySpecificTypeSpecifierTest, creationTest) {
  const PrimitiveTypeSpecifier* intSpecifer = PrimitiveTypes::INT->newTypeSpecifier(0);
  ArraySpecificTypeSpecifier* specifier =
  new ArraySpecificTypeSpecifier(intSpecifer, mDimensions, 11);
  const IType* type = specifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isReference());
  EXPECT_STREQ("int[]", type->getTypeName().c_str());
  
  const ArraySpecificType* arrayType = (const ArraySpecificType*) type;
  EXPECT_EQ(PrimitiveTypes::INT, arrayType->getElementType());
  EXPECT_EQ(1u, arrayType->getNumberOfDimensions());
  EXPECT_EQ(11, specifier->getLine());
}

TEST_F(ArraySpecificTypeSpecifierTest, printToStreamTest) {
  const PrimitiveTypeSpecifier* stringSpecifer = PrimitiveTypes::STRING->newTypeSpecifier(0);
  ArraySpecificTypeSpecifier* specifier =
  new ArraySpecificTypeSpecifier(stringSpecifer, mDimensions, 0);
  
  stringstream stringStream;
  specifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[3]", stringStream.str().c_str());
}
