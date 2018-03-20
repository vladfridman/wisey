//
//  IConcreteObjectDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Environment.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

void IConcreteObjectDefinition::configureObject(IRGenerationContext& context,
                                                IConcreteObjectType* object,
                                                vector<IObjectElementDefinition*>
                                                  elementDeclarations,
                                                vector<IInterfaceTypeSpecifier*>
                                                  interfaceSpecifiers) {
  vector<Interface*> interfaces = processInterfaces(context, interfaceSpecifiers);
  tuple<vector<Constant*>, vector<IField*>, vector<IMethod*>, vector<LLVMFunction*>> elements =
    createElements(context, object, elementDeclarations);
  object->setFields(get<1>(elements), interfaces.size() + 1);
  object->setInterfaces(interfaces);
  object->setMethods(get<2>(elements));
  object->setConstants(get<0>(elements));
  object->setLLVMFunctions(get<3>(elements));

  vector<llvm::Type*> types;

  // reference counter type
  llvm::Type* referenceCounterType = llvm::Type::getInt64Ty(context.getLLVMContext());
  types.push_back(referenceCounterType);
  
  for (Interface* interface : object->getInterfaces()) {
    types.push_back(interface->getLLVMType(context)->getPointerElementType());
  }
  
  collectFieldTypes(context, types, get<1>(elements));
  object->setStructBodyTypes(types);
  
  IConcreteObjectType::generateShortNameGlobal(context, object);
  IConcreteObjectType::generateNameGlobal(context, object);
  IConcreteObjectType::declareLLVMFunctions(context, object);
  IConcreteObjectType::generateVTable(context, object);
  IConcreteObjectType::generateConstantsIR(context, object);
}

tuple<vector<Constant*>, vector<IField*>, vector<IMethod*>, vector<LLVMFunction*>>
IConcreteObjectDefinition::createElements(IRGenerationContext& context,
                                          const IConcreteObjectType* concreteObjectType,
                                          vector<IObjectElementDefinition*> elementDeclarations) {
  vector<IField*> fields;
  vector<IMethod*> methods;
  vector<Constant*> constants;
  vector<LLVMFunction*> llvmFunctions;
  for (IObjectElementDefinition* elementDeclaration : elementDeclarations) {
    IObjectElement* element = elementDeclaration->define(context, concreteObjectType);
    if (element->isConstant()) {
      if (methods.size() || fields.size()) {
        Log::e("Constants should be declared before fields and methods");
        exit(1);
      }
      constants.push_back((Constant*) element);
    } else if (element->isField()) {
      if (methods.size()) {
        Log::e("Fields should be declared before methods");
        exit(1);
      }
      fields.push_back((IField*) element);
    } else if (element->isLLVMFunction()) {
      llvmFunctions.push_back((LLVMFunction*) element);
    } else {
      methods.push_back((IMethod*) element);
    }
  }
  
  return make_tuple(constants, fields, methods, llvmFunctions);
}

vector<Interface*> IConcreteObjectDefinition::processInterfaces(IRGenerationContext& context,
                                                                vector<IInterfaceTypeSpecifier*>
                                                                interfaceSpecifiers) {
  vector<Interface*> interfaces;
  for (IInterfaceTypeSpecifier* interfaceSpecifier : interfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    if (!interface->isComplete()) {
      const IObjectType* objectType = context.getObjectType();
      context.setObjectType(interface);
      interface->buildMethods(context);
      context.setObjectType(objectType);
    }
    interfaces.push_back(interface);
  }
  return interfaces;
}

void IConcreteObjectDefinition::collectFieldTypes(IRGenerationContext& context,
                                                  vector<llvm::Type*>& types,
                                                  vector<IField*> fields) {
  for (IField* field : fields) {
    const IType* fieldType = field->getType();
    
    if (field->isInjected() && fieldType->isInterface() && fieldType->isOwner()) {
      Interface* interface = (Interface*) fieldType->getObjectType();
      fieldType = context.getBoundController(interface);
    }
    
    llvm::Type* llvmType = fieldType->getLLVMType(context);
    types.push_back(llvmType);
  }
}

