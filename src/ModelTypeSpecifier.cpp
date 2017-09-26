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

std::string ModelTypeSpecifier::getShortName() const {
  return mShortName;
}

std::string ModelTypeSpecifier::getName(IRGenerationContext& context) const {
  return getFullName(context, mShortName, mPackage);
}

Model* ModelTypeSpecifier::getType(IRGenerationContext& context) const {
  return context.getModel(getName(context));
}

void ModelTypeSpecifier::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << getName(context);
}
