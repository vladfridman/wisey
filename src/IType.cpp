//
//  IType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/Log.hpp"

using namespace wisey;

bool IType::isObjectType(const IType* type) {
  return type->isController() || type->isInterface() || type->isModel() ||
  type->isNode() || type->isThread();
}

bool IType::isConcreteObjectType(const IType* type) {
  return isObjectType(type) && !type->isInterface();
}

void IType::repotNonInjectableType(IRGenerationContext& context, const IType* type, int line) {
  context.reportError(line, "type " + type->getTypeName() + " is not injectable");
}
