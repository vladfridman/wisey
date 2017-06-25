//
//  TestNodeTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NodeTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/NodeTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct NodeTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Node* mNode;
  vector<string> mPackage;
  
  NodeTypeSpecifierTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string nodeFullName = "systems.vos.wisey.compiler.tests.NElement";
    StructType* structType = StructType::create(llvmContext, nodeFullName);
    structType->setBody(types);
    mNode = new Node(nodeFullName, structType);
    mContext.addNode(mNode);
    
    mPackage.push_back("systems");
    mPackage.push_back("vos");
    mPackage.push_back("wisey");
    mPackage.push_back("compiler");
    mPackage.push_back("tests");
  }
};

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierCreateTest) {
  vector<string> package;
  NodeTypeSpecifier nodeTypeSpecifier(package, "NElement");
  
  EXPECT_EQ(nodeTypeSpecifier.getType(mContext), mNode);
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierCreateWithPackageTest) {
  NodeTypeSpecifier nodeTypeSpecifier(mPackage, "NElement");
  
  EXPECT_EQ(nodeTypeSpecifier.getType(mContext), mNode);
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierSamePackageDeathTest) {
  vector<string> package;
  NodeTypeSpecifier nodeTypeSpecifier(package, "NNode");
  
  EXPECT_EXIT(nodeTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Node systems.vos.wisey.compiler.tests.NNode is not defined");
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierNotDefinedDeathTest) {
  NodeTypeSpecifier nodeTypeSpecifier(mPackage, "NNode");
  
  EXPECT_EXIT(nodeTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Node systems.vos.wisey.compiler.tests.NNode is not defined");
}
