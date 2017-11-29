//
//  NodeTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Node.hpp"
#include "wisey/NodeTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NodeTypeSpecifier::NodeTypeSpecifier(string package, string shortName) :
mPackage(package),
mShortName(shortName) { }

NodeTypeSpecifier::~NodeTypeSpecifier() { }

string NodeTypeSpecifier::getShortName() const {
  return mShortName;
}

string NodeTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackage);
}

const Node* NodeTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getNode(getName(context));
}

void NodeTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}
