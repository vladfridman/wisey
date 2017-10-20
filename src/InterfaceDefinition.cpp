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

InterfaceDefinition::InterfaceDefinition(InterfaceTypeSpecifier* interfaceTypeSpecifier,
                                         vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                                         vector<IObjectElementDeclaration *>
                                         elementDeclarations) :
mInterfaceTypeSpecifier(interfaceTypeSpecifier),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mElementDeclarations(elementDeclarations) { }

InterfaceDefinition::~InterfaceDefinition() {
  delete mInterfaceTypeSpecifier;
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mParentInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mParentInterfaceSpecifiers.clear();
  for (IObjectElementDeclaration* elementDeclaration : mElementDeclarations) {
    delete elementDeclaration;
  }
  mElementDeclarations.clear();
}

void InterfaceDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mInterfaceTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Interface* interface = Interface::newInterface(fullName,
                                                 structType,
                                                 mParentInterfaceSpecifiers,
                                                 mElementDeclarations);
  context.addInterface(interface);
  
  interface->defineInterfaceTypeName(context);
}

void InterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  Interface* interface = context.getInterface(mInterfaceTypeSpecifier->getName(context));
  interface->buildMethods(context);
  interface->generateConstantsIR(context);
}

Value* InterfaceDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

