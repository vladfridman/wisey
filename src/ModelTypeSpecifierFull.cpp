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

ModelTypeSpecifierFull::ModelTypeSpecifierFull(string package, string shortName) :
mPackage(package),
mShortName(shortName) { }

ModelTypeSpecifierFull::~ModelTypeSpecifierFull() { }

string ModelTypeSpecifierFull::getShortName() const {
  return mShortName;
}

string ModelTypeSpecifierFull::getPackage() const {
  return mPackage;
}

string ModelTypeSpecifierFull::getName(IRGenerationContext& context) const {
  return mPackage + "." + mShortName;
}

Model* ModelTypeSpecifierFull::getType(IRGenerationContext& context) const {
  return context.getModel(getName(context));
}

void ModelTypeSpecifierFull::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getName(context);
}
