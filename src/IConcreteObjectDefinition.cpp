//
//  IConcreteObjectDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Environment.hpp"
#include "wisey/GetTypeNameMethod.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace wisey;

void IConcreteObjectDefinition::configureObject(IRGenerationContext& context,
                                                IConcreteObjectType* object,
                                                vector<IObjectElementDefinition*>
                                                  elementDeclarations,
                                                vector<IInterfaceTypeSpecifier*>
                                                interfaceSpecifiers,
                                                const IObjectTypeSpecifier* scopeObjectSpecifier) {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  vector<Interface*> interfaces = processInterfaces(context, object, interfaceSpecifiers);
  tuple<vector<Constant*>, vector<IField*>, vector<IMethod*>, vector<LLVMFunction*>> elements =
    createElements(context, object, elementDeclarations);
  unsigned long numberOfVtables = interfaces.size() ? interfaces.size() : 1u;
  unsigned long fieldStartIndex = object->isNode() ? numberOfVtables + 1 : numberOfVtables;
  object->setFields(context, get<1>(elements), fieldStartIndex);
  object->setInterfaces(interfaces);
  object->setMethods(get<2>(elements));
  object->setConstants(get<0>(elements));
  object->setLLVMFunctions(get<3>(elements));
  if (scopeObjectSpecifier) {
    object->setScopeType(scopeObjectSpecifier->getType(context));
  }

  vector<llvm::Type*> types;

  interfaces = object->getInterfaces();
  for (Interface* interface : interfaces) {
    types.push_back(interface->getLLVMType(context)->getPointerElementType());
  }
  if (interfaces.size() == 0) {
    llvm::Type* functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvmContext), true);
    llvm::Type* vtableType = functionType->getPointerTo()->getPointerTo();
    types.push_back(vtableType);
  }

  maybeAddPoolStore(context, types, object);
  collectFieldTypes(context, types, get<1>(elements));
  object->setStructBodyTypes(types);
  
  IConcreteObjectType::declareLLVMFunctions(context, object);
  
  if (object->isExternal()) {
    IConcreteObjectType::declareTypeNameGlobal(context, object);
    IConcreteObjectType::declareVTable(context, object);
    IConcreteObjectType::defineConstants(context, object);
    IConcreteObjectType::declareDestructor(context, object);
  } else {
    IConcreteObjectType::defineTypeNameGlobal(context, object);
    IConcreteObjectType::defineVTable(context, object);
    IConcreteObjectType::defineConstants(context, object);
  }
}

tuple<vector<Constant*>, vector<IField*>, vector<IMethod*>, vector<LLVMFunction*>>
IConcreteObjectDefinition::createElements(IRGenerationContext& context,
                                          const IConcreteObjectType* concreteObjectType,
                                          vector<IObjectElementDefinition*> elementDeclarations) {
  vector<IField*> fields;
  vector<IMethod*> methods;
  vector<Constant*> constants;
  vector<LLVMFunction*> llvmFunctions;
  map<string, IObjectElement*> nameToElementMap;
  for (IObjectElementDefinition* elementDeclaration : elementDeclarations) {
    IObjectElement* element = elementDeclaration->define(context, concreteObjectType);
    if (element->isConstant()) {
      if (methods.size() || fields.size()) {
        context.reportError(element->getLine(),
                            "Constants should be declared before fields and methods");
        throw 1;
      }
      Constant* constant = (Constant*) element;
      if (nameToElementMap.count(constant->getName())) {
        context.reportError(constant->getLine(), "Constant named '" + constant->getName() +
                            "' was already defined on line " +
                            to_string(nameToElementMap.at(constant->getName())->getLine()));
        throw 1;
      }
      nameToElementMap[constant->getName()] = constant;
      constants.push_back(constant);
    } else if (element->isField()) {
      if (methods.size()) {
        context.reportError(element->getLine(), "Fields should be declared before methods");
        throw 1;
      }
      IField* field = (IField*) element;
      if (nameToElementMap.count(field->getName())) {
        context.reportError(field->getLine(), "Field named '" + field->getName() +
                            "' was already defined on line " +
                            to_string(nameToElementMap.at(field->getName())->getLine()));
        throw 1;
      }
      nameToElementMap[field->getName()] = field;
      fields.push_back(field);
    } else if (element->isLLVMFunction()) {
      LLVMFunction* llvmFunction = (LLVMFunction*) element;
      if (nameToElementMap.count(llvmFunction->getName())) {
        context.reportError(llvmFunction->getLine(), "Function or method named '" +
                            llvmFunction->getName() + "' was already defined on line " +
                            to_string(nameToElementMap.at(llvmFunction->getName())->getLine()) +
                            ", method overloading is not allowed");
        throw 1;
      }
      nameToElementMap[llvmFunction->getName()] = llvmFunction;
      llvmFunctions.push_back((LLVMFunction*) element);
    } else {
      IMethod* method = (IMethod*) element;
      if (!method->getName().compare(GetTypeNameMethod::GET_TYPE_NAME_METHOD_NAME)) {
        context.reportError(method->getLine(), "Method named '" +
                            GetTypeNameMethod::GET_TYPE_NAME_METHOD_NAME +
                            "' is reserved and can not be overloaded");
        throw 1;
      }
      if (nameToElementMap.count(method->getName())) {
        context.reportError(method->getLine(), "Method named '" + method->getName() +
                            "' was already defined on line " +
                            to_string(nameToElementMap.at(method->getName())->getLine()) +
                            ", method overloading is not allowed");
        throw 1;
      }
      nameToElementMap[method->getName()] = method;
      methods.push_back(method);
    }
  }
  
  return make_tuple(constants, fields, methods, llvmFunctions);
}

vector<Interface*> IConcreteObjectDefinition::processInterfaces(IRGenerationContext& context,
                                                                const IConcreteObjectType*
                                                                concreteObjectType,
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
  context.setImportProfile(concreteObjectType->getImportProfile());
  return interfaces;
}

void IConcreteObjectDefinition::collectFieldTypes(IRGenerationContext& context,
                                                  vector<llvm::Type*>& types,
                                                  vector<IField*> fields) {
  for (IField* field : fields) {
    const IType* fieldType = field->getType();
    
    llvm::Type* llvmType = fieldType->getLLVMType(context);
    types.push_back(llvmType);
  }
}

void IConcreteObjectDefinition::maybeAddPoolStore(IRGenerationContext& context,
                                                  vector<llvm::Type*>& types,
                                                  const IConcreteObjectType* concreteObjectType) {
  if (!concreteObjectType->isNode()) {
    return;
  }
  
  const Controller* poolController = context.getController(Names::getCMemoryPoolFullName(), 0);
  types.push_back(poolController->getLLVMType(context));
}
