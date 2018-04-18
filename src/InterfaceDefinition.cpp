//
//  InterfaceDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/InstanceOf.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceDefinition::InterfaceDefinition(AccessLevel accessLevel,
                                         InterfaceTypeSpecifierFull* interfaceTypeSpecifierFull,
                                         vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                                         vector<IObjectElementDefinition *>
                                         elementDeclarations,
                                         vector<IObjectDefinition*> innerObjectDefinitions,
                                         int line) :
mAccessLevel(accessLevel),
mInterfaceTypeSpecifierFull(interfaceTypeSpecifierFull),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mElementDeclarations(elementDeclarations),
mInnerObjectDefinitions(innerObjectDefinitions),
mLine(line) { }

InterfaceDefinition::~InterfaceDefinition() {
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

Interface* InterfaceDefinition::prototypeObject(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mInterfaceTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Interface* interface = mAccessLevel == PUBLIC_ACCESS
  ? Interface::newPublicInterface(fullName,
                                  structType,
                                  mParentInterfaceSpecifiers,
                                  mElementDeclarations,
                                  mLine)
  : Interface::newPrivateInterface(fullName,
                                   structType,
                                   mParentInterfaceSpecifiers,
                                   mElementDeclarations,
                                   mLine);

  context.addInterface(interface);
  interface->setImportProfile(context.getImportProfile());
  interface->defineInterfaceTypeName(context);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(interface);
  IObjectDefinition::prototypeInnerObjects(context, interface, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return interface;
}

void InterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mInterfaceTypeSpecifierFull);
  Interface* interface = context.getInterface(fullName, mLine);
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(interface);

  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);

  interface->buildMethods(context);
  interface->generateConstantsIR(context);
  interface->defineLLVMFunctions(context);
  interface->defineStaticMethodFunctions(context);
  context.setObjectType(lastObjectType);
}

void InterfaceDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mInterfaceTypeSpecifierFull);
  Interface* interface = context.getInterface(fullName, mLine);

  context.getScopes().pushScope();
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(interface);

  IObjectDefinition::generateInnerObjectIR(context, mInnerObjectDefinitions);
  interface->defineCurrentObjectNameVariable(context);
  interface->generateLLVMFunctionsIR(context);
  interface->generateStaticMethodsIR(context);

  context.setObjectType(lastObjectType);
  context.getScopes().popScope(context, 0);
}
