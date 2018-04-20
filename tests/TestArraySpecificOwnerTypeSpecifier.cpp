//
//  TestArraySpecificOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 2/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArraySpecificOwnerTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "MockExpression.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/ArraySpecificOwnerTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArraySpecificOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  list<const IExpression*> mDimensions;
  NiceMock<MockExpression> mThreeExpression;

  ArraySpecificOwnerTypeSpecifierTest() : mLLVMContext(mContext.getLLVMContext()) {
    llvm::Constant* three = llvm::ConstantInt::get(llvm::Type::getInt64Ty(mLLVMContext), 3);
    ON_CALL(mThreeExpression, generateIR(_, _)).WillByDefault(Return(three));
    ON_CALL(mThreeExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(mThreeExpression, printToStream(_, _)).WillByDefault(Invoke(printThree));
    mDimensions.push_back(&mThreeExpression);
  }
  
  static void printThree(IRGenerationContext& context, iostream& stream) {
    stream << "3";
  }
};

TEST_F(ArraySpecificOwnerTypeSpecifierTest, creationTest) {
  const PrimitiveTypeSpecifier* intSpecifer = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
  ArraySpecificTypeSpecifier* specifier =
  new ArraySpecificTypeSpecifier(intSpecifer, mDimensions, 9);
  ArraySpecificOwnerTypeSpecifier* ownerSpecifier = new ArraySpecificOwnerTypeSpecifier(specifier);
  const IType* type = ownerSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("int[]*", type->getTypeName().c_str());
  
  const ArraySpecificOwnerType* arraySpecificOwnerType = (const ArraySpecificOwnerType*) type;
  const ArraySpecificType* arraySpecificType = arraySpecificOwnerType->getArraySpecificType();
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, arraySpecificType->getElementType());
  EXPECT_EQ(1u, arraySpecificType->getNumberOfDimensions());
  EXPECT_EQ(9, ownerSpecifier->getLine());
}

TEST_F(ArraySpecificOwnerTypeSpecifierTest, twoGetsReturnNotSameTypeObjectTest) {
  const PrimitiveTypeSpecifier* floatSpecifer = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier(0);
  ArraySpecificTypeSpecifier* specifier =
  new ArraySpecificTypeSpecifier(floatSpecifer, mDimensions, 0);
  ArraySpecificOwnerTypeSpecifier* ownerSpecifier = new ArraySpecificOwnerTypeSpecifier(specifier);
  const IType* type1 = ownerSpecifier->getType(mContext);
  const IType* type2 = ownerSpecifier->getType(mContext);
  
  EXPECT_NE(type1, type2);
}

TEST_F(ArraySpecificOwnerTypeSpecifierTest, printToStreamTest) {
  const PrimitiveTypeSpecifier* stringSpecifer = PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  ArraySpecificTypeSpecifier* specifier =
  new ArraySpecificTypeSpecifier(stringSpecifer, mDimensions, 0);
  ArraySpecificOwnerTypeSpecifier* ownerSpecifier = new ArraySpecificOwnerTypeSpecifier(specifier);

  stringstream stringStream;
  ownerSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[3]*", stringStream.str().c_str());
}

