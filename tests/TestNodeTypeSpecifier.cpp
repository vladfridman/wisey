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
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  NodeTypeSpecifierTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string nodeFullName = "systems.vos.wisey.compiler.tests.NElement";
    StructType* structType = StructType::create(llvmContext, nodeFullName);
    structType->setBody(types);
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS, nodeFullName, structType);
    mContext.addNode(mNode);
  }
};

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierCreateTest) {
  NodeTypeSpecifier nodeTypeSpecifier("", "NElement");
  
  EXPECT_EQ(nodeTypeSpecifier.getType(mContext), mNode);
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierCreateWithPackageTest) {
  NodeTypeSpecifier nodeTypeSpecifier(mPackage, "NElement");
  
  EXPECT_EQ(nodeTypeSpecifier.getType(mContext), mNode);
}

TEST_F(NodeTypeSpecifierTest, printToStreamTest) {
  NodeTypeSpecifier nodeTypeSpecifier(mPackage, "NElement");

  stringstream stringStream;
  nodeTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.NElement", stringStream.str().c_str());
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierSamePackageDeathTest) {
  NodeTypeSpecifier nodeTypeSpecifier("", "NNode");
  
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
