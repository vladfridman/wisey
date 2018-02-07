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
  PrimitiveTypeSpecifier* intSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  ArraySpecificTypeSpecifier* specifier = new ArraySpecificTypeSpecifier(intSpecifer, mDimensions);
  ArraySpecificOwnerTypeSpecifier* ownerSpecifier = new ArraySpecificOwnerTypeSpecifier(specifier);
  const IType* type = ownerSpecifier->getType(mContext);
  
  EXPECT_EQ(ARRAY_OWNER_TYPE, type->getTypeKind());
  EXPECT_STREQ("int[]*", type->getTypeName().c_str());
  
  const ArraySpecificOwnerType* arraySpecificOwnerType = (const ArraySpecificOwnerType*) type;
  const ArraySpecificType* arraySpecificType = arraySpecificOwnerType->getArraySpecificType();
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, arraySpecificType->getElementType());
  EXPECT_EQ(1u, arraySpecificType->getNumberOfDimensions());
}

TEST_F(ArraySpecificOwnerTypeSpecifierTest, twoGetsReturnNotSameTypeObjectTest) {
  PrimitiveTypeSpecifier* floatSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  ArraySpecificTypeSpecifier* specifier = new ArraySpecificTypeSpecifier(floatSpecifer,
                                                                         mDimensions);
  ArraySpecificOwnerTypeSpecifier* ownerSpecifier = new ArraySpecificOwnerTypeSpecifier(specifier);
  const IType* type1 = ownerSpecifier->getType(mContext);
  const IType* type2 = ownerSpecifier->getType(mContext);
  
  EXPECT_NE(type1, type2);
}

TEST_F(ArraySpecificOwnerTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier* stringSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  ArraySpecificTypeSpecifier* specifier = new ArraySpecificTypeSpecifier(stringSpecifer,
                                                                         mDimensions);
  ArraySpecificOwnerTypeSpecifier* ownerSpecifier = new ArraySpecificOwnerTypeSpecifier(specifier);

  stringstream stringStream;
  ownerSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[3]*", stringStream.str().c_str());
}

