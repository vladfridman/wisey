//
//  InterfaceDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Interface.hpp"
#include "yazyk/InterfaceDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* InterfaceDefinition::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<MethodSignature*> methodSignatures;
  unsigned int methodIndex = 0;
  for (MethodSignatureDeclaration* methodSignatureDeclaration : mMethodSignatureDeclarations) {
    MethodSignature* methodSignature =
      methodSignatureDeclaration->createMethodSignature(context, methodIndex);
    methodSignatures.push_back(methodSignature);
    methodIndex++;
  }
  
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  vector<Type*> types;
  types.push_back(vtableType);
  
  vector<Interface*> parentInterfaces;
  for (string parentInterfaceName : mParentInterfaceNames) {
    Interface* parentInterface = context.getInterface(parentInterfaceName);
    types.push_back(parentInterface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType());
    parentInterfaces.push_back(parentInterface);
  }
  
  StructType *structType = StructType::create(llvmContext, "interface." + mName);
  structType->setBody(types);
  Interface* interface = new Interface(mName, structType, parentInterfaces, methodSignatures);
  context.addInterface(interface);
  
  return NULL;
}
