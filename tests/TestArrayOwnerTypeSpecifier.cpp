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

#include "ArrayOwnerTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ArrayOwnerTypeSpecifierTest() { }
};

TEST_F(ArrayOwnerTypeSpecifierTest, creationTest) {
  const PrimitiveTypeSpecifier* intSpecifer = PrimitiveTypes::INT->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(intSpecifer, 1u, 9);
  ArrayOwnerTypeSpecifier* specifier = new ArrayOwnerTypeSpecifier(arraySpecifier);
  const IType* type = specifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("int[]*", type->getTypeName().c_str());
  
  const ArrayOwnerType* arrayType = (const ArrayOwnerType*) type;
  EXPECT_EQ(PrimitiveTypes::INT, arrayType->getArrayType(mContext, 0)->getElementType());
  EXPECT_EQ(1u, arrayType->getArrayType(mContext, 0)->getNumberOfDimensions());
  EXPECT_EQ(9, specifier->getLine());
}

TEST_F(ArrayOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const PrimitiveTypeSpecifier* floatSpecifer = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(floatSpecifer, 1u, 0);
  ArrayOwnerTypeSpecifier* specifier = new ArrayOwnerTypeSpecifier(arraySpecifier);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ArrayOwnerTypeSpecifierTest, printToStreamTest) {
  const PrimitiveTypeSpecifier* stringSpecifer = PrimitiveTypes::STRING->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(stringSpecifer, 1u, 0);
  ArrayOwnerTypeSpecifier* specifier = new ArrayOwnerTypeSpecifier(arraySpecifier);

  stringstream stringStream;
  specifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("string[]*", stringStream.str().c_str());
}
