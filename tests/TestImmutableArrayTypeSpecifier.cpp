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
#include <gmock/gmock.h>

#include "MockType.hpp"
#include "MockTypeSpecifier.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "ImmutableArrayTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ImmutableArrayTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  ImmutableArrayTypeSpecifierTest() {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(ImmutableArrayTypeSpecifierTest, creationTest) {
  const PrimitiveTypeSpecifier* intSpecifer = PrimitiveTypes::INT->newTypeSpecifier(0);
  ArrayTypeSpecifier* arrayTypeSpecifier = new ArrayTypeSpecifier(intSpecifer, 1u, 3);
  ImmutableArrayTypeSpecifier* specifier = new ImmutableArrayTypeSpecifier(arrayTypeSpecifier);
  const ImmutableArrayType* type = specifier->getType(mContext);
  
  EXPECT_TRUE(type->isArray());
  EXPECT_TRUE(type->isReference());
  EXPECT_STREQ("immutable int[]", type->getTypeName().c_str());
  
  EXPECT_EQ(PrimitiveTypes::INT, type->getElementType());
  EXPECT_EQ(1u, type->getNumberOfDimensions());
  EXPECT_EQ(3, specifier->getLine());
}

TEST_F(ImmutableArrayTypeSpecifierTest, getTypeDeathTest) {
  NiceMock<MockTypeSpecifier> mockTypeSpecifier;
  NiceMock<MockType> mockType;
  Mock::AllowLeak(&mockTypeSpecifier);
  Mock::AllowLeak(&mockType);

  ON_CALL(mockTypeSpecifier, getType(_)).WillByDefault(Return(&mockType));
  EXPECT_CALL(mockType, die());
  ArrayTypeSpecifier* arrayTypeSpecifier = new ArrayTypeSpecifier(&mockTypeSpecifier, 1u, 3);
  ImmutableArrayTypeSpecifier* specifier = new ImmutableArrayTypeSpecifier(arrayTypeSpecifier);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(specifier->getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Immutable array base type can only be of primitive or immutable type\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ImmutableArrayTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const PrimitiveTypeSpecifier* floatSpecifer = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  ArrayTypeSpecifier* arrayTypeSpecifier = new ArrayTypeSpecifier(floatSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* specifier = new ImmutableArrayTypeSpecifier(arrayTypeSpecifier);
  const IType* type1 = specifier->getType(mContext);
  const IType* type2 = specifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(ImmutableArrayTypeSpecifierTest, printToStreamTest) {
  const PrimitiveTypeSpecifier* stringSpecifer = PrimitiveTypes::STRING->newTypeSpecifier(0);
  ArrayTypeSpecifier* arrayTypeSpecifier = new ArrayTypeSpecifier(stringSpecifer, 1u, 0);
  ImmutableArrayTypeSpecifier* specifier = new ImmutableArrayTypeSpecifier(arrayTypeSpecifier);

  stringstream stringStream;
  specifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("immutable string[]", stringStream.str().c_str());
}

TEST_F(TestFileRunner, immutableArrayNodeBaseRunDeathTest) {
  expectFailCompile("tests/samples/test_immutable_array_node_base.yz",
                    1,
                    "tests/samples/test_immutable_array_node_base.yz\\(12\\): "
                    "Error: Immutable array base type can only be of primitive or immutable type");
}
