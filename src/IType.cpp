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
  typeKind == CONTROLLER_OWNER_TYPE ||
  typeKind == NODE_OWNER_TYPE;
}

bool IType::isReferenceType(const IType* type) {
  TypeKind typeKind = type->getTypeKind();
  
  return typeKind == MODEL_TYPE ||
  typeKind == INTERFACE_TYPE ||
  typeKind == CONTROLLER_TYPE ||
  typeKind == NODE_TYPE;
}

bool IType::isConcreteObjectType(const IType* type) {
  TypeKind typeKind = type->getTypeKind();
  
  return typeKind == MODEL_TYPE || typeKind == CONTROLLER_TYPE || typeKind == NODE_TYPE;
}

bool IType::isArrayType(const IType* type) {
  TypeKind typeKind = type->getTypeKind();
  
  return typeKind == ARRAY_OWNER_TYPE || typeKind == ARRAY_TYPE;
}
