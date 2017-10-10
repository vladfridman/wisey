//
//  IMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/HeapOwnerMethodParameter.hpp"
#include "wisey/HeapReferenceMethodParameter.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StackVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IMethod::storeArgumentValue(IRGenerationContext& context,
                                 string variableName,
                                 const IType* variableType,
                                 Value* variableValue) {
  if (variableType->getTypeKind() == PRIMITIVE_TYPE) {
    LLVMContext& llvmContext = context.getLLVMContext();
    Type* llvmType = variableType->getLLVMType(llvmContext);
    string newName = variableName + ".param";
    AllocaInst *alloc = IRWriter::newAllocaInst(context, llvmType, newName);
    IRWriter::newStoreInst(context, variableValue, alloc);
    IVariable* variable = new StackVariable(variableName, variableType, alloc);
    context.getScopes().setVariable(variable);
    return;
  }
  
  if (IType::isOwnerType(variableType)) {
    Type* variableLLVMType = variableType->getLLVMType(context.getLLVMContext());
    Value* alloc = IRWriter::newAllocaInst(context, variableLLVMType, "parameterObjectPointer");
    IRWriter::newStoreInst(context, variableValue, alloc);
    IObjectOwnerType* objectOwnerType = (IObjectOwnerType*) variableType;
    IVariable* variable = new HeapOwnerMethodParameter(variableName, objectOwnerType, alloc);
    context.getScopes().setVariable(variable);
    return;
  }
  
  IVariable* variable = new HeapReferenceMethodParameter(variableName,
                                                         (IObjectType*) variableType,
                                                         variableValue);
  context.getScopes().setVariable(variable);
}

void IMethod::checkForUnhandledExceptions(IRGenerationContext& context, const IMethod* method) {
  Scope* scope = context.getScopes().getScope();
  map<string, const Model*> exceptions = scope->getExceptions();
  if (exceptions.size() == 0) {
    return;
  }
  
  for (const Model* thrownException : method->getThrownExceptions()) {
    scope->removeException(thrownException);
  }
  
  exceptions = scope->getExceptions();
  bool hasUnhandledExceptions = false;
  for (map<string, const Model*>::const_iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    Log::e((method->isStatic() ? "Static method " : "Method ") + method->getName() +
           " neither handles the exception " + iterator->first + " nor throws it");
    hasUnhandledExceptions = true;
  }
  
  if (hasUnhandledExceptions) {
    exit(1);
  }
}

void IMethod::maybeAddImpliedVoidReturn(IRGenerationContext& context, const IMethod* method) {
  if (context.getBasicBlock()->getTerminator()) {
    return;
  }
  
  const IType* returnType = method->getReturnType();
  if (returnType != PrimitiveTypes::VOID_TYPE) {
    Log::e((method->isStatic() ? "Static method " : "Method ") + method->getName() +
           " must return a value of type " + returnType->getName());
    exit(1);
  }
  
  context.getScopes().freeOwnedMemory(context);
  IRWriter::createReturnInst(context, NULL);
}
