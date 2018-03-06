//
//  IType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IType.hpp"

using namespace wisey;

bool IType::isObjectType(const IType* type) {
  return type->isController() || type->isInterface() || type->isModel() || type->isNode();
}

bool IType::isConcreteObjectType(const IType* type) {
  return type->isController() || type->isModel() || type->isNode();
}
