//
//  IType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IType.hpp"

using namespace wisey;

bool IType::isOwnerType(const IType* type) {
  TypeKind typeKind = type->getTypeKind();
  
  return typeKind == MODEL_OWNER_TYPE ||
    typeKind == INTERFACE_OWNER_TYPE ||
    typeKind == CONTROLLER_OWNER_TYPE;
}
