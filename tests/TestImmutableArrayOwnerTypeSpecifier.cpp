//
//  TestImmutableArrayOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ImmutableArrayOwnerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "ImmutableArrayOwnerTypeSpecifier.hpp"
#include "ImmutableArrayTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ImmutableArrayOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ImmutableArrayOwnerTypeSpecifierTest() { }
};

TEST_F(ImmutableArrayOwnerTypeSpecifierTest, creationTest) {
  const PrimitiveTypeSpecifier* intSpecifer = PrimitiveTypes::INT->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(intSpecifer, 1u, 9);
  ImmutableArrayTypeSpecifier* immutableArray = new ImmutableArrayTypeSpecifier(arraySpecifier);
  ImmutableArrayOwnerTypeSpecifier* specifier =
    new ImmutableArrayOwnerTypeSpecifier(immutableArray);
  const ImmutableArrayOwnerType* type = specifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("immutable int[]*", type->getTypeName().c_str());
  
  EXPECT_EQ(PrimitiveTypes::INT, type->getArrayType(mContext, 0)->getElementType());
  EXPECT_EQ(1u, type->getArrayType(mContext, 0)->getNumberOfDimensions());
  EXPECT_EQ(9, specifier->getLine());
}

TEST_F(ImmutableArrayOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const PrimitiveTypeSpecifier* floatSpecifer = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(floatSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* immutableArray = new ImmutableArrayTypeSpecifier(arraySpecifier);
  ImmutableArrayOwnerTypeSpecifier* specifier =
    new ImmutableArrayOwnerTypeSpecifier(immutableArray);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ImmutableArrayOwnerTypeSpecifierTest, printToStreamTest) {
  const PrimitiveTypeSpecifier* stringSpecifer = PrimitiveTypes::STRING->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(stringSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* immutableArray = new ImmutableArrayTypeSpecifier(arraySpecifier);
  ImmutableArrayOwnerTypeSpecifier* specifier =
    new ImmutableArrayOwnerTypeSpecifier(immutableArray);

  stringstream stringStream;
  specifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("immutable string[]*", stringStream.str().c_str());
}
