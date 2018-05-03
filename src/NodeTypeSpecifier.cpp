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

NodeTypeSpecifier::NodeTypeSpecifier(IExpression* packageExpression, string shortName, int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) { }

NodeTypeSpecifier::~NodeTypeSpecifier() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string NodeTypeSpecifier::getShortName() const {
  return mShortName;
}

IExpression* NodeTypeSpecifier::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string NodeTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackageExpression, mLine);
}

const Node* NodeTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getNode(getName(context), mLine);
}

void NodeTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

int NodeTypeSpecifier::getLine() const {
  return mLine;
}
