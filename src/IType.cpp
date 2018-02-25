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

bool IType::isPrimitveType(const IType* type) {
  return type->getTypeKind() == PRIMITIVE_TYPE;
}

