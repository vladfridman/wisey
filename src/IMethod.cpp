//
//  IMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IMethod.hpp"
#include "wisey/IPrimitiveType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterArrayOwnerVariable.hpp"
#include "wisey/ParameterArrayReferenceVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IMethod::storeArgumentValue(IRGenerationContext& context,
                                 string variableName,
                                 const IType* variableType,
                                 Value* variableValue) {
  IVariable* variable = NULL;
  
  if (IType::isPrimitveType(variableType)) {
    const IPrimitiveType* primitiveType = (const IPrimitiveType*) variableType;
    variable = new ParameterPrimitiveVariable(variableName, primitiveType, variableValue);
    
  } else if (variableType->getTypeKind() == ARRAY_OWNER_TYPE) {
    Type* variableLLVMType = variableType->getLLVMType(context);
    Value* alloc = IRWriter::newAllocaInst(context, variableLLVMType, "parameterArrayPointer");
    IRWriter::newStoreInst(context, variableValue, alloc);
    ArrayOwnerType* arrayOwnerType = (ArrayOwnerType*) variableType;
    variable = new ParameterArrayOwnerVariable(variableName, arrayOwnerType, alloc);
    
  } else if (variableType->getTypeKind() == ARRAY_TYPE) {
    ArrayType* arrayType = (ArrayType*) variableType;
    variable = new ParameterArrayReferenceVariable(variableName, arrayType,  variableValue);
    arrayType->incrementReferenceCount(context, variableValue);

  } else if (IType::isOwnerType(variableType)) {
    Type* variableLLVMType = variableType->getLLVMType(context);
    Value* alloc = IRWriter::newAllocaInst(context, variableLLVMType, "parameterObjectPointer");
    IRWriter::newStoreInst(context, variableValue, alloc);
    IObjectOwnerType* objectOwnerType = (IObjectOwnerType*) variableType;
    variable = new ParameterOwnerVariable(variableName, objectOwnerType, alloc);
    
  } else {
    assert(IType::isReferenceType(variableType));
    IObjectType* referenceType = (IObjectType*) variableType;
    variable = new ParameterReferenceVariable(variableName, referenceType, variableValue);
    referenceType->incrementReferenceCount(context, variableValue);
  }
  
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
    if (!iterator->first.find(Names::getLangPackageName())) {
      continue;
    }
    Log::e((method->isStatic() ? "Static method " : "Method ") + method->getName() +
           " neither handles the exception " + iterator->first + " nor throws it");
    hasUnhandledExceptions = true;
  }
  
  if (hasUnhandledExceptions) {
    exit(1);
  }
}

void IMethod::maybeAddImpliedVoidReturn(IRGenerationContext& context,
                                        const IMethod* method,
                                        int line) {
  if (context.getBasicBlock()->getTerminator()) {
    return;
  }
  
  const IType* returnType = method->getReturnType();
  if (returnType != PrimitiveTypes::VOID_TYPE) {
    Log::e((method->isStatic() ? "Static method " : "Method ") + method->getName() +
           " must return a value of type " + returnType->getTypeName());
    exit(1);
  }
  
  context.getScopes().freeOwnedMemory(context, line);
  IRWriter::createReturnInst(context, NULL);
}

Function* IMethod::defineFunction(IRGenerationContext& context,
                                  const IObjectType* objectType,
                                  const IMethod* method) {
  FunctionType* ftype = method->getLLVMType(context);
  string methodName = method->getName();
  string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType,
                                                                             methodName);
  
  GlobalValue::LinkageTypes linkageType = method->getAccessLevel() == PRIVATE_ACCESS
    ? GlobalValue::InternalLinkage
    : GlobalValue::ExternalLinkage;
  Function* function = Function::Create(ftype, linkageType, functionName, context.getModule());
  
  string functionNameConstantName = MethodCall::getMethodNameConstantName(methodName);
  if (!context.getModule()->getNamedGlobal(functionNameConstantName)) {
    llvm::Constant* stringConstant =
      ConstantDataArray::getString(context.getLLVMContext(), methodName);
    new GlobalVariable(*context.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::InternalLinkage,
                       stringConstant,
                       functionNameConstantName);
  }
  
  return function;
}

void IMethod::defineCurrentMethodNameVariable(IRGenerationContext& context, string methodName) {
  string constantName = IMethodCall::getMethodNameConstantName(methodName);
  GlobalVariable* constant = context.getModule()->getNamedGlobal(constantName);
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = constant->getType()->getPointerElementType();
  
  Value* value = ConstantExpr::getGetElementPtr(elementType, constant, Idx);
  
  ParameterPrimitiveVariable* methodNameVariable =
  new ParameterPrimitiveVariable(Names::getCurrentMethodVariableName(),
                                 PrimitiveTypes::STRING_TYPE,
                                 value);
  context.getScopes().setVariable(methodNameVariable);
}
