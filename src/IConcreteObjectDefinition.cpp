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
                                                vector<IObjectElementDeclaration*>
                                                  elementDeclarations,
                                                vector<InterfaceTypeSpecifier*>
                                                  interfaceSpecifiers) {
  vector<Interface*> interfaces = processInterfaces(context, interfaceSpecifiers);
  tuple<vector<Constant*>, vector<Field*>, vector<IMethod*>> elements =
    createElements(context, elementDeclarations);
  object->setFields(get<1>(elements), interfaces.size() + 1);
  object->setInterfaces(interfaces);
  object->setMethods(get<2>(elements));
  object->setConstants(get<0>(elements));
  
  vector<llvm::Type*> types;

  // reference counter type
  llvm::Type* referenceCounterType = llvm::Type::getInt64Ty(context.getLLVMContext());
  types.push_back(referenceCounterType);
  
  for (Interface* interface : object->getInterfaces()) {
    types.push_back(interface->getLLVMType(context.getLLVMContext())->getPointerElementType());
  }
  
  collectFieldTypes(context, types, get<1>(elements));
  object->setStructBodyTypes(types);
  
  IConcreteObjectType::generateNameGlobal(context, object);
  IConcreteObjectType::generateVTable(context, object);
  IConcreteObjectType::generateConstantsIR(context, object);
}

tuple<vector<Constant*>, vector<Field*>, vector<IMethod*>>
IConcreteObjectDefinition::createElements(IRGenerationContext& context,
                                          vector<IObjectElementDeclaration*>
                                          elementDeclarations) {
  vector<Field*> fields;
  vector<IMethod*> methods;
  vector<Constant*> constants;
  for (IObjectElementDeclaration* elementDeclaration : elementDeclarations) {
    IObjectElement* element = elementDeclaration->declare(context);
    if (element->getObjectElementType() == OBJECT_ELEMENT_CONSTANT) {
      if (methods.size() || fields.size()) {
        Log::e("Constants should be declared before fields and methods");
        exit(1);
      }
      constants.push_back((Constant*) element);
    } else if (element->getObjectElementType() == OBJECT_ELEMENT_FIELD) {
      if (methods.size()) {
        Log::e("Fields should be declared before methods");
        exit(1);
      }
      fields.push_back((Field*) element);
    } else {
      methods.push_back((IMethod*) element);
    }
  }
  
  return make_tuple(constants, fields, methods);
}

vector<Interface*> IConcreteObjectDefinition::processInterfaces(IRGenerationContext& context,
                                                                vector<InterfaceTypeSpecifier*>
                                                                interfaceSpecifiers) {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : interfaceSpecifiers) {
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
                                                  vector<Field*> fields) {
  llvm::LLVMContext& llvmContext = context.getLLVMContext();
  
  for (Field* field : fields) {
    const IType* fieldType = field->getType();
    
    if (field->getFieldKind() == INJECTED_FIELD &&
        fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
      Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
      fieldType = context.getBoundController(interface);
    }
    
    llvm::Type* llvmType = fieldType->getLLVMType(llvmContext);
    types.push_back(llvmType);
  }
}

