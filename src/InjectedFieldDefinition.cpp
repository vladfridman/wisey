//
//  InjectedFieldDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/InjectedFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InjectedFieldDefinition::InjectedFieldDefinition(const ITypeSpecifier* typeSpecifier,
                                                 string name,
                                                 InjectionArgumentList injectionArguments,
                                                 bool isImmediate,
                                                 int line) :
mTypeSpecifier(typeSpecifier),
mName(name),
mInjectionArguments(injectionArguments),
mIsImmediate(isImmediate),
mLine(line) { }

InjectedFieldDefinition::~InjectedFieldDefinition() {
  delete mTypeSpecifier;
  for (InjectionArgument* injectionArgument : mInjectionArguments) {
    delete injectionArgument;
  }
  mInjectionArguments.clear();
}

InjectedFieldDefinition* InjectedFieldDefinition::
createDelayed(const ITypeSpecifier* typeSpecifier,
              std::string name,
              InjectionArgumentList injectionArguments,
              int line) {
  return new InjectedFieldDefinition(typeSpecifier, name, injectionArguments, false, line);
}

InjectedFieldDefinition* InjectedFieldDefinition::
createImmediate(const ITypeSpecifier* typeSpecifier,
                std::string name,
                InjectionArgumentList injectionArguments,
                int line) {
  return new InjectedFieldDefinition(typeSpecifier, name, injectionArguments, true, line);
}

IField* InjectedFieldDefinition::define(IRGenerationContext& context,
                                        const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  const IType* injectedType = NULL;
  if (fieldType->isArray() && fieldType->isOwner()) {
    injectedType = fieldType;
    fieldType = ((const ArraySpecificOwnerType*) fieldType)->getArraySpecificType()
    ->getArrayType(context, mLine)->getOwner();
  } else {
    injectedType = fieldType;
  }
  
  string sourceFile = context.getImportProfile()->getSourceFileName();
  InjectedField* injectedField = mIsImmediate
  ? InjectedField::createImmediate(fieldType,
                                   injectedType,
                                   mName,
                                   mInjectionArguments,
                                   sourceFile,
                                   mLine)
  : InjectedField::createDelayed(fieldType,
                                 injectedType,
                                 mName,
                                 mInjectionArguments,
                                 sourceFile,
                                 mLine);
  injectedField->checkType(context);
  
  return injectedField;
}

bool InjectedFieldDefinition::isConstant() const {
  return false;
}

bool InjectedFieldDefinition::isField() const {
  return true;
}

bool InjectedFieldDefinition::isMethod() const {
  return false;
}

bool InjectedFieldDefinition::isStaticMethod() const {
  return false;
}

bool InjectedFieldDefinition::isMethodSignature() const {
  return false;
}

bool InjectedFieldDefinition::isLLVMFunction() const {
  return false;
}
