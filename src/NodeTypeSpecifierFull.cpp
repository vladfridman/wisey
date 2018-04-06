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

NodeTypeSpecifierFull::NodeTypeSpecifierFull(IExpression* packageExpression,
                                             string shortName,
                                             int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) { }

NodeTypeSpecifierFull::~NodeTypeSpecifierFull() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string NodeTypeSpecifierFull::getShortName() const {
  return mShortName;
}

IExpression* NodeTypeSpecifierFull::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string NodeTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackageExpression->getType(context)->getTypeName() + "." + mShortName;
}

Node* NodeTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getNode(getName(context), mLine);
}

void NodeTypeSpecifierFull::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

