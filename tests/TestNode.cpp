//
//  TestNode.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Node}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/Node.hpp"
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

struct NodeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mNode;
  StructType* mStructType;
  
  NodeTest() : mLLVMContext(mContext.getLLVMContext()) {
    string nodeFullName = "systems.vos.wisey.compiler.tests.NElement";
    mStructType = StructType::create(mLLVMContext, nodeFullName);
    mNode = new Node(nodeFullName, mStructType);
  }
};

TEST_F(NodeTest, testGetOwner) {
  ASSERT_NE(mNode->getOwner(), nullptr);
  EXPECT_EQ(mNode->getOwner()->getObject(), mNode);
}
