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

#include "wisey/ArraySpecificTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArraySpecificTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ArraySpecificTypeSpecifierTest() { }
};

TEST_F(ArraySpecificTypeSpecifierTest, creationTest) {
  PrimitiveTypeSpecifier* intSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  vector<unsigned long> dimensions;
  dimensions.push_back(3u);
  ArraySpecificTypeSpecifier* specifier = new ArraySpecificTypeSpecifier(intSpecifer, dimensions);
  const IType* type = specifier->getType(mContext);
  
  EXPECT_EQ(ARRAY_TYPE, type->getTypeKind());
  EXPECT_STREQ("int[3]", type->getTypeName().c_str());
  
  const ArraySpecificType* arrayType = (const ArraySpecificType*) type;
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, arrayType->getElementType());
  EXPECT_EQ(3u, arrayType->getDimensions().front());
}

TEST_F(ArraySpecificTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  PrimitiveTypeSpecifier* floatSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<unsigned long> dimensions;
  dimensions.push_back(3u);
  ArraySpecificTypeSpecifier* specifier = new ArraySpecificTypeSpecifier(floatSpecifer, dimensions);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ArraySpecificTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier* stringSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  vector<unsigned long> dimensions;
  dimensions.push_back(5u);
  ArraySpecificTypeSpecifier specifier(stringSpecifer, dimensions);
  
  stringstream stringStream;
  specifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[5]", stringStream.str().c_str());
}
