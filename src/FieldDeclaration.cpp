//
//  FieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Field.hpp"
#include "wisey/FieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FieldDeclaration::~FieldDeclaration() {
  delete mTypeSpecifier;
  for (IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
}

Field* FieldDeclaration::declare(IRGenerationContext& context, unsigned long index) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  if (mFieldKind == INJECTED_FIELD && fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
    Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
    fieldType = context.getBoundController(interface)->getOwner();
  }
  
  return new Field(mFieldKind, fieldType, mName, index, mArguments);
}

FieldKind FieldDeclaration::getFieldKind() const {
  return mFieldKind;
}

ITypeSpecifier* FieldDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

string FieldDeclaration::getName() const {
  return mName;
}

ExpressionList FieldDeclaration::getArguments() const {
  return mArguments;
}
