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

ExternalInterfaceDefinition::ExternalInterfaceDefinition(InterfaceTypeSpecifierFull*
                                                         interfaceTypeSpecifierFull,
                                                         vector<IInterfaceTypeSpecifier*>
                                                         parentInterfaceSpecifiers,
                                                         vector<IObjectElementDeclaration *>
                                                         elementDeclarations) :
mInterfaceTypeSpecifierFull(interfaceTypeSpecifierFull),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mElementDeclarations(elementDeclarations) { }

ExternalInterfaceDefinition::~ExternalInterfaceDefinition() {
  delete mInterfaceTypeSpecifierFull;
  for (IInterfaceTypeSpecifier* interfaceTypeSpecifier : mParentInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mParentInterfaceSpecifiers.clear();
  for (IObjectElementDeclaration* elementDeclaration : mElementDeclarations) {
    delete elementDeclaration;
  }
  mElementDeclarations.clear();
}

void ExternalInterfaceDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mInterfaceTypeSpecifierFull->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Interface* interface = Interface::newExternalInterface(fullName,
                                                         structType,
                                                         mParentInterfaceSpecifiers,
                                                         mElementDeclarations);
  context.addInterface(interface);
  interface->setImportProfile(context.getImportProfile());

  interface->defineInterfaceTypeName(context);
}

void ExternalInterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  Interface* interface = context.getInterface(mInterfaceTypeSpecifierFull->getName(context));
  context.setObjectType(interface);
  interface->buildMethods(context);
  context.setObjectType(NULL);
}

Value* ExternalInterfaceDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

