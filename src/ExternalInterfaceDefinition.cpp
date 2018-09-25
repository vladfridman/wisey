//
//  ExternalInterfaceDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ExternalInterfaceDefinition.hpp"
#include "Interface.hpp"
#include "MethodSignatureDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalInterfaceDefinition::ExternalInterfaceDefinition(InterfaceTypeSpecifierFull*
                                                         interfaceTypeSpecifierFull,
                                                         vector<IInterfaceTypeSpecifier*>
                                                         parentInterfaceSpecifiers,
                                                         vector<IObjectElementDefinition *>
                                                         elementDeclarations,
                                                         vector<IObjectDefinition*>
                                                         innerObjectDefinitions,
                                                         int line) :
mInterfaceTypeSpecifierFull(interfaceTypeSpecifierFull),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mElementDeclarations(elementDeclarations),
mInnerObjectDefinitions(innerObjectDefinitions),
mLine(line) { }

ExternalInterfaceDefinition::~ExternalInterfaceDefinition() {
  delete mInterfaceTypeSpecifierFull;
  for (IInterfaceTypeSpecifier* interfaceTypeSpecifier : mParentInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mParentInterfaceSpecifiers.clear();
  for (IObjectElementDefinition* elementDeclaration : mElementDeclarations) {
    delete elementDeclaration;
  }
  mElementDeclarations.clear();
  for (IObjectDefinition* innerObjectDefinition : mInnerObjectDefinitions) {
    delete innerObjectDefinition;
  }
  mInnerObjectDefinitions.clear();
}

Interface* ExternalInterfaceDefinition::prototypeObject(IRGenerationContext& context,
                                                        ImportProfile* importProfile) const {
  string fullName = IObjectDefinition::getFullName(context, mInterfaceTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Interface* interface = Interface::newExternalInterface(fullName,
                                                         structType,
                                                         mParentInterfaceSpecifiers,
                                                         mElementDeclarations,
                                                         importProfile,
                                                         mLine);
  context.addInterface(interface, mLine);
  interface->declareInterfaceTypeName(context);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(interface);
  IObjectDefinition::prototypeInnerObjects(context,
                                           importProfile,
                                           interface,
                                           mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return interface;
}

void ExternalInterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mInterfaceTypeSpecifierFull);
  Interface* interface = context.getInterface(fullName, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(interface);
  interface->defineExternalInjectionFunctionPointer(context, mLine);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  interface->buildMethods(context);
  interface->defineConstants(context);
  interface->defineStaticMethodFunctions(context);
  context.setObjectType(lastObjectType);
}

void ExternalInterfaceDefinition::generateIR(IRGenerationContext& context) const {
}

