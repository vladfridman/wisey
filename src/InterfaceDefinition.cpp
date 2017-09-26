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
  Interface* interface = new Interface(fullName, structType);
  context.addInterface(interface);
}

void InterfaceDefinition::prototypeMethods(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Interface* interface = context.getInterface(mInterfaceTypeSpecifier->getName(context));

  vector<MethodSignature*> methodSignatures;
  for (MethodSignatureDeclaration* methodSignatureDeclaration : mMethodSignatureDeclarations) {
    MethodSignature* methodSignature =
    methodSignatureDeclaration->createMethodSignature(context);
    methodSignatures.push_back(methodSignature);
  }
  
  vector<Interface*> parentInterfaces;
  for (InterfaceTypeSpecifier* parentInterfaceSpecifier : mParentInterfaceSpecifiers) {
    Interface* parentInterface = (Interface*) parentInterfaceSpecifier->getType(context);
    parentInterfaces.push_back(parentInterface);
  }

  interface->setParentInterfacesAndMethodSignatures(parentInterfaces, methodSignatures);

  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  vector<Type*> types;
  types.push_back(vtableType);
  
  for (Interface* parentInterface : interface->getParentInterfaces()) {
    types.push_back(parentInterface->getLLVMType(llvmContext)
                    ->getPointerElementType()->getPointerElementType());
  }
  
  interface->setStructBodyTypes(types);
  
  defineInterfaceTypeName(context, interface);
}

Value* InterfaceDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

void InterfaceDefinition::defineInterfaceTypeName(IRGenerationContext& context,
                                                  Interface* interface) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, interface->getName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());
}

