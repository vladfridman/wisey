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
  StructType *structType = StructType::create(llvmContext, "interface." + mName);
  
  vector<Type*> types;
  map<string, Method*>* methods = new map<string, Method*>();
  Interface* interface = new Interface(mName, structType, methods);
  
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  types.push_back(vtableType);
  structType->setBody(types);
  
  for (vector<MethodSignature *>::iterator iterator = mMethods.begin();
       iterator != mMethods.end();
       iterator++) {
    MethodSignature* methodSignature = *iterator;
    Method* method = methodSignature->getMethod(context);
    (*methods)[method->getName()] = method;
  }
  
  context.addInterface(interface);
  
  return NULL;
}
