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

#include "wisey/FakeExpression.hpp"
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
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                          nodeFullName,
                          structType,
                          mContext.getImportProfile(),
                          0);
    mContext.addNode(mNode);
  }
};

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierCreateTest) {
  NodeTypeSpecifier nodeTypeSpecifier(NULL, "NElement", 0);
  
  EXPECT_EQ(nodeTypeSpecifier.getType(mContext), mNode);
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierCreateWithPackageTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifier nodeTypeSpecifier(packageExpression, "NElement", 0);
  
  EXPECT_EQ(nodeTypeSpecifier.getType(mContext), mNode);
}

TEST_F(NodeTypeSpecifierTest, printToStreamTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifier nodeTypeSpecifier(packageExpression, "NElement", 0);

  stringstream stringStream;
  nodeTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.NElement", stringStream.str().c_str());
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierSamePackageDeathTest) {
  NodeTypeSpecifier nodeTypeSpecifier(NULL, "NNode", 10);
  
  EXPECT_EXIT(nodeTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Node systems.vos.wisey.compiler.tests.NNode is not defined");
}

TEST_F(NodeTypeSpecifierTest, nodeTypeSpecifierNotDefinedDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifier nodeTypeSpecifier(packageExpression, "NNode", 15);
  
  EXPECT_EXIT(nodeTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Node systems.vos.wisey.compiler.tests.NNode is not defined");
}
