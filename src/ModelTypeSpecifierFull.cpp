//
//  ModelTypeSpecifierFull.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ModelTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelTypeSpecifierFull::ModelTypeSpecifierFull(IExpression* packageExpression, string shortName) :
mPackageExpression(packageExpression),
mShortName(shortName) {
}

ModelTypeSpecifierFull::~ModelTypeSpecifierFull() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string ModelTypeSpecifierFull::getShortName() const {
  return mShortName;
}

IExpression* ModelTypeSpecifierFull::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string ModelTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackageExpression->getType(context)->getTypeName() + "." + mShortName;
}

Model* ModelTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getModel(getName(context));
}

void ModelTypeSpecifierFull::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}
