//
//  TestNodeOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NodeOwner}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/NodeOwner.hpp"
#include "wisey/NodeTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NodeOwnerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mNode;
  StructType* mStructType;
  
  NodeOwnerTest() : mLLVMContext(mContext.getLLVMContext()) {
    string nodeFullName = "systems.vos.wisey.compiler.tests.NElement";
    mStructType = StructType::create(mLLVMContext, nodeFullName);
    mNode = new Node(nodeFullName, mStructType);
  }
};

TEST_F(NodeOwnerTest, getObjectTest) {
  EXPECT_EQ(mNode->getOwner()->getObject(), mNode);
}

TEST_F(NodeOwnerTest, getNameTest) {
  EXPECT_STREQ(mNode->getOwner()->getName().c_str(),
               "systems.vos.wisey.compiler.tests.NElement*");
}

TEST_F(NodeOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mNode->getOwner()->getLLVMType(mLLVMContext),
            mNode->getLLVMType(mLLVMContext));
}

TEST_F(NodeOwnerTest, getTypeKindTest) {
  EXPECT_EQ(mNode->getOwner()->getTypeKind(), NODE_OWNER_TYPE);
}
