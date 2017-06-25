//
//  TestNodeDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NodeDefinition}
//

#include <gtest/gtest.h>

#include "wisey/NodeDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct NodeDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  
  NodeDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
  }
  
  ~NodeDefinitionTest() {
  }
};
