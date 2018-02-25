//
//  TestArrayTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/ArrayTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ArrayTypeSpecifierTest() { }
};

TEST_F(ArrayTypeSpecifierTest, creationTest) {
  PrimitiveTypeSpecifier* intSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  ArrayTypeSpecifier* specifier = new ArrayTypeSpecifier(intSpecifer, 1u);
  const IType* type = specifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isReference());
  EXPECT_STREQ("int[]", type->getTypeName().c_str());
  
  const ArrayType* arrayType = (const ArrayType*) type;
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, arrayType->getElementType());
  EXPECT_EQ(1u, arrayType->getNumberOfDimensions());
}

TEST_F(ArrayTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  PrimitiveTypeSpecifier* floatSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  ArrayTypeSpecifier* specifier = new ArrayTypeSpecifier(floatSpecifer, 1u);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ArrayTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier* stringSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  ArrayTypeSpecifier specifier(stringSpecifer, 1u);

  stringstream stringStream;
  specifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[]", stringStream.str().c_str());
}
