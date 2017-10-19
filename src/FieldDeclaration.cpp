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

FieldDeclaration::FieldDeclaration(FieldKind fieldKind,
                                   ITypeSpecifier* typeSpecifier,
                                   string name,
                                   ExpressionList arguments) :
mFieldKind(fieldKind),
mTypeSpecifier(typeSpecifier),
mName(name),
mArguments(arguments) { }

FieldDeclaration::~FieldDeclaration() {
  delete mTypeSpecifier;
  for (IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
}

Field* FieldDeclaration::declare(IRGenerationContext& context) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  if (mFieldKind == INJECTED_FIELD && fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
    Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
    fieldType = context.getBoundController(interface)->getOwner();
  }
  
  return new Field(mFieldKind, fieldType, mName, mArguments);
}

