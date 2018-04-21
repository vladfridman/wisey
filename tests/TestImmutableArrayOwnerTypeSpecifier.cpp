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

#include "wisey/ImmutableArrayOwnerTypeSpecifier.hpp"
#include "wisey/ImmutableArrayTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ImmutableArrayOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ImmutableArrayOwnerTypeSpecifierTest() { }
};

TEST_F(ImmutableArrayOwnerTypeSpecifierTest, creationTest) {
  const PrimitiveTypeSpecifier* intSpecifer = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(intSpecifer, 1u, 9);
  ImmutableArrayTypeSpecifier* immutableArray = new ImmutableArrayTypeSpecifier(arraySpecifier);
  ImmutableArrayOwnerTypeSpecifier* specifier =
    new ImmutableArrayOwnerTypeSpecifier(immutableArray);
  const ImmutableArrayOwnerType* type = specifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("immutable int[]*", type->getTypeName().c_str());
  
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, type->getArrayType(mContext)->getElementType());
  EXPECT_EQ(1u, type->getArrayType(mContext)->getNumberOfDimensions());
  EXPECT_EQ(9, specifier->getLine());
}

TEST_F(ImmutableArrayOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const PrimitiveTypeSpecifier* floatSpecifer = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(floatSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* immutableArray = new ImmutableArrayTypeSpecifier(arraySpecifier);
  ImmutableArrayOwnerTypeSpecifier* specifier =
    new ImmutableArrayOwnerTypeSpecifier(immutableArray);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ImmutableArrayOwnerTypeSpecifierTest, printToStreamTest) {
  const PrimitiveTypeSpecifier* stringSpecifer = PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  ArrayTypeSpecifier* arraySpecifier = new ArrayTypeSpecifier(stringSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* immutableArray = new ImmutableArrayTypeSpecifier(arraySpecifier);
  ImmutableArrayOwnerTypeSpecifier* specifier =
    new ImmutableArrayOwnerTypeSpecifier(immutableArray);

  stringstream stringStream;
  specifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("immutable string[]*", stringStream.str().c_str());
}
