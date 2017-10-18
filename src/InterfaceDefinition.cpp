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
                                         vector<MethodSignatureDeclaration *>
                                         methodSignatureDeclarations) :
mInterfaceTypeSpecifier(interfaceTypeSpecifier),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mMethodSignatureDeclarations(methodSignatureDeclarations) { }

InterfaceDefinition::~InterfaceDefinition() {
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

void InterfaceDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mInterfaceTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Interface* interface = Interface::newInterface(fullName,
                                                 structType,
                                                 mParentInterfaceSpecifiers,
                                                 mMethodSignatureDeclarations);
  context.addInterface(interface);
  
  interface->defineInterfaceTypeName(context);
}

void InterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  Interface* interface = context.getInterface(mInterfaceTypeSpecifier->getName(context));
  interface->buildMethods(context);
}

Value* InterfaceDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

