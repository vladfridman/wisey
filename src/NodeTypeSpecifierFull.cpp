//
//  NodeTypeSpecifierFull.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/NodeTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NodeTypeSpecifierFull::NodeTypeSpecifierFull(string package, string shortName) :
mPackage(package),
mShortName(shortName) { }

NodeTypeSpecifierFull::~NodeTypeSpecifierFull() { }

string NodeTypeSpecifierFull::getShortName() const {
  return mShortName;
}

string NodeTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackage + "." + mShortName;
}

Node* NodeTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getNode(getName(context));
}

void NodeTypeSpecifierFull::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

