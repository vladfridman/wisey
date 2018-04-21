//
//  TestImmutableArrayTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ImmutableArrayTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/ImmutableArrayTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ImmutableArrayTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ImmutableArrayTypeSpecifierTest() { }
};

TEST_F(ImmutableArrayTypeSpecifierTest, creationTest) {
  const PrimitiveTypeSpecifier* intSpecifer = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
  ArrayTypeSpecifier* arrayTypeSpecifier = new ArrayTypeSpecifier(intSpecifer, 1u, 3);
  ImmutableArrayTypeSpecifier* specifier = new ImmutableArrayTypeSpecifier(arrayTypeSpecifier);
  const ImmutableArrayType* type = specifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isReference());
  EXPECT_STREQ("immutable int[]", type->getTypeName().c_str());
  
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, type->getElementType());
  EXPECT_EQ(1u, type->getNumberOfDimensions());
  EXPECT_EQ(3, specifier->getLine());
}

TEST_F(ImmutableArrayTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const PrimitiveTypeSpecifier* floatSpecifer = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier(0);
  ArrayTypeSpecifier* arrayTypeSpecifier = new ArrayTypeSpecifier(floatSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* specifier = new ImmutableArrayTypeSpecifier(arrayTypeSpecifier);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ImmutableArrayTypeSpecifierTest, printToStreamTest) {
  const PrimitiveTypeSpecifier* stringSpecifer = PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  ArrayTypeSpecifier* arrayTypeSpecifier = new ArrayTypeSpecifier(stringSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* specifier = new ImmutableArrayTypeSpecifier(arrayTypeSpecifier);

  stringstream stringStream;
  specifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("immutable string[]", stringStream.str().c_str());
}
