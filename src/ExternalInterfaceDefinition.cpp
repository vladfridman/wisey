//
//  ExternalInterfaceDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ExternalInterfaceDefinition.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/Interface.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalInterfaceDefinition::~ExternalInterfaceDefinition() {
  delete mInterfaceTypeSpecifier;
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mParentInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mParentInterfaceSpecifiers.clear();
  for (MethodSignatureDeclaration* methodSignatureDeclaration : mMethodSignatureDeclarations) {
    delete methodSignatureDeclaration;
  }
  mMethodSignatureDeclarations.clear();
}

void ExternalInterfaceDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mInterfaceTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Interface* interface = new Interface(fullName,
                                       structType,
                                       mParentInterfaceSpecifiers,
                                       mMethodSignatureDeclarations);
  context.addInterface(interface);
  
  interface->defineInterfaceTypeName(context);
}

void ExternalInterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  Interface* interface = context.getInterface(mInterfaceTypeSpecifier->getName(context));
  interface->buildMethods(context);
}

Value* ExternalInterfaceDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

