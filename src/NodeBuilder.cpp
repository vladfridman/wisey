//
//  NodeBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Node.hpp"
#include "wisey/NodeBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NodeBuilder::~NodeBuilder() {
  delete mNodeTypeSpecifier;
  for (BuilderArgument* argument : mBuilderArgumentList) {
    delete argument;
  }
  mBuilderArgumentList.clear();
}

Value* NodeBuilder::generateIR(IRGenerationContext& context) const {
  // TODO: implement this
  return NULL;
}

void NodeBuilder::releaseOwnership(IRGenerationContext& context) const {
  // TODO: implement this
}

const IType* NodeBuilder::getType(IRGenerationContext& context) const {
  return mNodeTypeSpecifier->getType(context)->getOwner();
}

bool NodeBuilder::existsInOuterScope(IRGenerationContext& context) const {
  // TODO: implement this
  return false;
}
