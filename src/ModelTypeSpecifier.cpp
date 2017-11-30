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

ModelTypeSpecifier::ModelTypeSpecifier(string package, string shortName) :
mPackage(package),
mShortName(shortName) { }

ModelTypeSpecifier::~ModelTypeSpecifier() { }

string ModelTypeSpecifier::getShortName() const {
  return mShortName;
}

string ModelTypeSpecifier::getPackage() const {
  return mPackage;
}

string ModelTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackage);
}

Model* ModelTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getModel(getName(context));
}

void ModelTypeSpecifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}
