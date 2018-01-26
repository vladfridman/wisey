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
  vector<unsigned long> dimensions;
  dimensions.push_back(3u);
  ArrayTypeSpecifier* specifier = new ArrayTypeSpecifier(intSpecifer, dimensions);
  const IType* type = specifier->getType(mContext);
  
  EXPECT_EQ(ARRAY_TYPE, type->getTypeKind());
  EXPECT_STREQ("int[3]", type->getTypeName().c_str());
  
  const ArrayType* arrayType = (const ArrayType*) type;
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, arrayType->getElementType());
  EXPECT_EQ(3u, arrayType->getDimensions().front());
}

TEST_F(ArrayTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  PrimitiveTypeSpecifier* floatSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<unsigned long> dimensions;
  dimensions.push_back(3u);
  ArrayTypeSpecifier* specifier = new ArrayTypeSpecifier(floatSpecifer, dimensions);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ArrayTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier* stringSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  vector<unsigned long> dimensions;
  dimensions.push_back(5u);
  ArrayTypeSpecifier specifier(stringSpecifer, dimensions);

  stringstream stringStream;
  specifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[5]", stringStream.str().c_str());
}

