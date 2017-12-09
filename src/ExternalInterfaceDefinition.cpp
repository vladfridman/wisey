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
                                                         elementDeclarations,
                                                         vector<IObjectDefinition*>
                                                         innerObjectDefinitions) :
mInterfaceTypeSpecifierFull(interfaceTypeSpecifierFull),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mElementDeclarations(elementDeclarations),
mInnerObjectDefinitions(innerObjectDefinitions) { }

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
  for (IObjectDefinition* innerObjectDefinition : mInnerObjectDefinitions) {
    delete innerObjectDefinition;
  }
  mInnerObjectDefinitions.clear();
}

Interface* ExternalInterfaceDefinition::prototypeObject(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mInterfaceTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Interface* interface = Interface::newExternalInterface(fullName,
                                                         structType,
                                                         mParentInterfaceSpecifiers,
                                                         mElementDeclarations);
  context.addInterface(interface);
  interface->setImportProfile(context.getImportProfile());
  interface->defineInterfaceTypeName(context);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(interface);
  IObjectDefinition::prototypeInnerObjects(context, interface, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return interface;
}

void ExternalInterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mInterfaceTypeSpecifierFull);
  Interface* interface = context.getInterface(fullName);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(interface);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  interface->buildMethods(context);
  interface->defineStaticMethodFunctions(context);
  context.setObjectType(lastObjectType);
}

Value* ExternalInterfaceDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

