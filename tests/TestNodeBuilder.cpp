//
//  TestNodeBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NodeBuilder}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/NodeBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NodeBuilderTest : Test {
  IRGenerationContext mContext;
  Node* mNode;
  NodeBuilder* mNodeBuilder;
  NodeTypeSpecifier* mNodeTypeSpecifier;
  BasicBlock *mBlock;
  
  NodeBuilderTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    vector<string> package;
    mNodeTypeSpecifier = new NodeTypeSpecifier(package, "NElement");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string nodeFullName = "systems.vos.wisey.compiler.tests.NElement";
    StructType *structType = StructType::create(llvmContext, nodeFullName);
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["mWidth"] = new Field(PrimitiveTypes::INT_TYPE, "mWidth", 0, fieldArguments);
    fields["mHeight"] = new Field(PrimitiveTypes::INT_TYPE, "mHeight", 1, fieldArguments);
    mNode = new Node(nodeFullName, structType);
    mContext.addNode(mNode);
  }
  
  ~NodeBuilderTest() {
  }
};

TEST_F(NodeBuilderTest, testGetType) {
  BuilderArgumentList argumentList;
  NodeBuilder nodeBuilder(mNodeTypeSpecifier, argumentList);
  
  EXPECT_EQ(nodeBuilder.getType(mContext), mNode->getOwner());
}
