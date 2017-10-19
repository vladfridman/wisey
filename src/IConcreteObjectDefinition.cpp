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

void IConcreteObjectDefinition::configureObject(IRGenerationContext& context,
                                                IConcreteObjectType* object,
                                                vector<IObjectElementDeclaration*>
                                                  elementDeclarations,
                                                vector<InterfaceTypeSpecifier*>
                                                  interfaceSpecifiers) {
  vector<Interface*> interfaces = processInterfaces(context, interfaceSpecifiers);
  tuple<vector<Field*>, vector<IMethod*>> elements = createElements(context, elementDeclarations);
  object->setFields(get<0>(elements), interfaces.size());
  
  object->setInterfaces(interfaces);
  object->setMethods(get<1>(elements));
  
  vector<Type*> types;
  for (Interface* interface : object->getInterfaces()) {
    types.push_back(interface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType()->getPointerElementType());
  }
  
  collectFieldTypes(context, types, get<0>(elements));
  object->setStructBodyTypes(types);
  
  IConcreteObjectType::generateNameGlobal(context, object);
  IConcreteObjectType::generateVTable(context, object);
}

tuple<vector<Field*>, vector<IMethod*>>
IConcreteObjectDefinition::createElements(IRGenerationContext& context,
                                          vector<IObjectElementDeclaration*>
                                          elementDeclarations) {
  vector<Field*> fields;
  vector<IMethod*> methods;
  for (IObjectElementDeclaration* elementDeclaration : elementDeclarations) {
    IObjectElement* element = elementDeclaration->declare(context);
    if (element->getObjectElementType() == OBJECT_ELEMENT_FIELD) {
      fields.push_back((Field*) element);
    } else {
      methods.push_back((IMethod*) element);
    }
  }
  
  return make_tuple(fields, methods);
}

vector<Interface*> IConcreteObjectDefinition::processInterfaces(IRGenerationContext& context,
                                                                vector<InterfaceTypeSpecifier*>
                                                                interfaceSpecifiers) {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : interfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    if (!interface->isComplete()) {
      interface->buildMethods(context);
    }
    interfaces.push_back(interface);
  }
  return interfaces;
}

void IConcreteObjectDefinition::collectFieldTypes(IRGenerationContext& context,
                                                  vector<Type*>& types,
                                                  vector<Field*> fields) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (Field* field : fields) {
    const IType* fieldType = field->getType();
    
    if (field->getFieldKind() == INJECTED_FIELD &&
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

