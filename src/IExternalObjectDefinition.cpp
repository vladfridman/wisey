//
//  IExternalObjectDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IExternalObjectDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodSignature.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IExternalObjectDefinition::configureExternalObject(IRGenerationContext& context,
                                                        IConcreteObjectType* object,
                                                        vector<FieldDeclaration*> fieldDeclarations,
                                                        vector<MethodSignatureDeclaration*>
                                                        methodSignatures,
                                                        vector<InterfaceTypeSpecifier*>
                                                        interfaceSpecifiers) {
  vector<Field*> fields = createFields(context, fieldDeclarations, interfaceSpecifiers.size());
  object->setFields(fields);
  
  vector<Interface*> interfaces = processInterfaces(context, interfaceSpecifiers);
  vector<IMethod*> methods = createExternalMethods(context, methodSignatures);
  
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

vector<IMethod*> IExternalObjectDefinition::createExternalMethods(IRGenerationContext& context,
                                                                  vector<MethodSignatureDeclaration*>
                                                                  methodSignatures) {
  vector<IMethod*> methods;
  for (MethodSignatureDeclaration* methodSignature : methodSignatures) {
    IMethod* method = methodSignature->createExternalMethod(context);
    methods.push_back(method);
  }
  return methods;
}
