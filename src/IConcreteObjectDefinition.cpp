//
//  IConcreteObjectDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IConcreteObjectDefinition::configureConcreteObject(IRGenerationContext& context,
                                                        IConcreteObjectType* object,
                                                        vector<FieldDeclaration*>
                                                        fieldDeclarations,
                                                        vector<IMethodDeclaration*>
                                                        methodDeclarations,
                                                        vector<InterfaceTypeSpecifier*>
                                                        interfaceSpecifiers) {
  vector<Interface*> interfaces = processInterfaces(context, interfaceSpecifiers);
  vector<IMethod*> methods = createMethods(context, methodDeclarations);
  
  object->setInterfaces(interfaces);
  object->setMethods(methods);
  
  vector<Type*> types;
  for (Interface* interface : object->getInterfaces()) {
    types.push_back(interface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType()->getPointerElementType());
  }
  
  collectFieldTypes(context, types, fieldDeclarations);
  object->setStructBodyTypes(types);
  
  IConcreteObjectType::generateNameGlobal(context, object);
  IConcreteObjectType::generateVTable(context, object);
}

vector<Interface*> IConcreteObjectDefinition::processInterfaces(IRGenerationContext& context,
                                                                vector<InterfaceTypeSpecifier*>
                                                                interfaceSpecifiers) {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : interfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    interfaces.push_back(interface);
  }
  return interfaces;
}

vector<IMethod*> IConcreteObjectDefinition::createMethods(IRGenerationContext& context,
                                                          vector<IMethodDeclaration*>
                                                          methodDeclarations) {
  vector<IMethod*> methods;
  for (IMethodDeclaration* methodDeclaration : methodDeclarations) {
    IMethod* method = methodDeclaration->createMethod(context);
    methods.push_back(method);
  }
  return methods;
}

void IConcreteObjectDefinition::collectFieldTypes(IRGenerationContext& context,
                                                  vector<Type*>& types,
                                                  vector<FieldDeclaration*> fieldDeclarations) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (FieldDeclaration* fieldDeclaration : fieldDeclarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    if (fieldDeclaration->getFieldKind() == INJECTED_FIELD &&
        fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
      Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
      fieldType = context.getBoundController(interface);
    }
    
    Type* llvmType = fieldType->getLLVMType(llvmContext);
    if (IType::isReferenceType(fieldType)) {
      types.push_back(llvmType->getPointerElementType());
    } else {
      types.push_back(llvmType);
    }
  }
}
