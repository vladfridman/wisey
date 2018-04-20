//
//  ModelTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ModelTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelTypeSpecifier::ModelTypeSpecifier(IExpression* packageExpression, string shortName, int line) :
mPackageExpression(packageExpression),
mShortName(shortName),
mLine(line) {
}

ModelTypeSpecifier::~ModelTypeSpecifier() {
  if (mPackageExpression) {
    delete mPackageExpression;
  }
}

string ModelTypeSpecifier::getShortName() const {
  return mShortName;
}

IExpression* ModelTypeSpecifier::takePackage() {
  IExpression* packageExpression = mPackageExpression;
  mPackageExpression = NULL;
  return packageExpression;
}

string ModelTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackageExpression);
}

Model* ModelTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getModel(getName(context), mLine);
}

void ModelTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}

int ModelTypeSpecifier::getLine() const {
  return mLine;
}
