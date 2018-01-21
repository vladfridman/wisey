//
//  TestArrayOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayOwnerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/ArrayOwnerTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ArrayOwnerTypeSpecifierTest() { }
};

TEST_F(ArrayOwnerTypeSpecifierTest, creationTest) {
  PrimitiveTypeSpecifier* intSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(intSpecifer, 3u);
  ArrayOwnerTypeSpecifier* specifier = new ArrayOwnerTypeSpecifier(arraySpecifier);
  const IType* type = specifier->getType(mContext);
  
  EXPECT_EQ(ARRAY_OWNER_TYPE, type->getTypeKind());
  EXPECT_STREQ("int[3]*", type->getTypeName().c_str());
  
  const ArrayOwnerType* arrayType = (const ArrayOwnerType*) type;
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, arrayType->getBaseType());
  EXPECT_EQ(3u, arrayType->getSize());
}

TEST_F(ArrayOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  PrimitiveTypeSpecifier* floatSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(floatSpecifer, 3u);
  ArrayOwnerTypeSpecifier* specifier = new ArrayOwnerTypeSpecifier(arraySpecifier);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ArrayOwnerTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier* stringSpecifer = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(stringSpecifer, 5u);
  ArrayOwnerTypeSpecifier* specifier = new ArrayOwnerTypeSpecifier(arraySpecifier);

  stringstream stringStream;
  specifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[5]*", stringStream.str().c_str());
}

