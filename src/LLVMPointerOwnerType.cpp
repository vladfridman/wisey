//
//  LLVMPointerOwnerType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "ArrayType.hpp"
#include "DestroyNativeObjectFunction.hpp"
#include "FieldOwnerVariable.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LLVMFunction.hpp"
#include "LLVMPointerOwnerType.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "LocalOwnerVariable.hpp"
#include "ParameterOwnerVariable.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMPointerOwnerType::LLVMPointerOwnerType(const LLVMPointerType* pointerType) :
mPointerType(pointerType) {
}

LLVMPointerOwnerType::~LLVMPointerOwnerType() {
}

string LLVMPointerOwnerType::getTypeName() const {
  return mPointerType->getTypeName() + "*";
}

llvm::PointerType* LLVMPointerOwnerType::getLLVMType(IRGenerationContext& context) const {
  return mPointerType->getLLVMType(context);
}

bool LLVMPointerOwnerType::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this) {
    return true;
  }
  return mPointerType->canCastTo(context, toType);
  return false;
}

bool LLVMPointerOwnerType::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

llvm::Value* LLVMPointerOwnerType::castTo(IRGenerationContext& context,
                                          llvm::Value* fromValue,
                                          const IType* toType,
                                          int line) const {
  if (toType == this) {
    return fromValue;
  }
  return mPointerType->castTo(context, fromValue, toType, line);
}

bool LLVMPointerOwnerType::isPrimitive() const {
  return false;
}

bool LLVMPointerOwnerType::isOwner() const {
  return true;
}

bool LLVMPointerOwnerType::isReference() const {
  return false;
}

bool LLVMPointerOwnerType::isArray() const {
  return false;
}

bool LLVMPointerOwnerType::isFunction() const {
  return false;
}

bool LLVMPointerOwnerType::isPackage() const {
  return false;
}

bool LLVMPointerOwnerType::isController() const {
  return false;
}

bool LLVMPointerOwnerType::isInterface() const {
  return false;
}

bool LLVMPointerOwnerType::isModel() const {
  return false;
}

bool LLVMPointerOwnerType::isNode() const {
  return false;
}

bool LLVMPointerOwnerType::isNative() const {
  return true;
}

bool LLVMPointerOwnerType::isPointer() const {
  return true;
}

bool LLVMPointerOwnerType::isImmutable() const {
  return false;
}

void LLVMPointerOwnerType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMPointerOwnerType::createLocalVariable(IRGenerationContext& context,
                                               string name,
                                               int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  llvm::Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  IVariable* variable = new LocalOwnerVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMPointerOwnerType::createFieldVariable(IRGenerationContext& context,
                                               string name,
                                               const IConcreteObjectType* object,
                                               int line) const {
  IVariable* variable = new FieldOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void LLVMPointerOwnerType::createParameterVariable(IRGenerationContext& context,
                                                   string name,
                                                   llvm::Value* value,
                                                   int line) const {
  llvm::PointerType::Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* LLVMPointerOwnerType::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

const IReferenceType* LLVMPointerOwnerType::getReference() const {
  return NULL;
}

void LLVMPointerOwnerType::free(IRGenerationContext& context,
                                Value* value,
                                llvm::Value* exception,
                                const LLVMFunction* customDestructor,
                                int line) const {
  if (!customDestructor) {
    DestroyNativeObjectFunction::call(context, value);
    return;
  }
  
  vector<Value*> arguments;
  arguments.push_back(value);
  Function* destructorFunction = customDestructor->getLLVMFunction(context);
  IRWriter::createCallInst(context, destructorFunction, arguments, "");
}

Instruction* LLVMPointerOwnerType::inject(IRGenerationContext& context,
                                          const InjectionArgumentList injectionArgumentList,
                                          int line) const {
  checkInjectionArguments(context, injectionArgumentList, line);
  vector<Value*> callArguments;
  InjectionArgument* constructorArgument = getConstructorArgument(injectionArgumentList);
  const LLVMFunction* constructor = (const LLVMFunction*) constructorArgument->getType(context);
  Function* llvmFunction = constructor->getLLVMFunction(context);

  return IRWriter::createCallInst(context, llvmFunction, callArguments, "");
}

void LLVMPointerOwnerType::checkInjectionArguments(IRGenerationContext &context,
                                                   const InjectionArgumentList injectionArgumentList,
                                                   int line) const {
  if (injectionArgumentList.size() != 2) {
    context.reportError(line, "Both constructor and destructor must be provided "
                        "for pointer injection");
    throw 1;
  }
  InjectionArgument* constructorArgument = getConstructorArgument(injectionArgumentList);
  InjectionArgument* destructorArgument = getDestructorArgument(injectionArgumentList);
  
  if (!constructorArgument) {
    context.reportError(line, "Constructor function is not specified");
    throw 1;
  }
  if (!destructorArgument) {
    context.reportError(line, "Destructor function is not specified");
    throw 1;
  }
  
  checkConstructorType(context, constructorArgument->getType(context), line);
  checkDestructorType(context, destructorArgument->getType(context), line);
}

InjectionArgument* LLVMPointerOwnerType::
getConstructorArgument(const InjectionArgumentList injectionArgumentList) const {
  if (!injectionArgumentList.front()->deriveFieldName().compare("mConstructor")) {
    return injectionArgumentList.front();
  } else if (!injectionArgumentList.back()->deriveFieldName().compare("mConstructor")) {
    return injectionArgumentList.back();
  }
  return NULL;
}

InjectionArgument* LLVMPointerOwnerType::
getDestructorArgument(const InjectionArgumentList injectionArgumentList) const {
  if (!injectionArgumentList.front()->deriveFieldName().compare("mDestructor")) {
    return injectionArgumentList.front();
  } else if (!injectionArgumentList.back()->deriveFieldName().compare("mDestructor")) {
    return injectionArgumentList.back();
  }
  return NULL;
}

void LLVMPointerOwnerType::checkConstructorType(IRGenerationContext& context,
                                                const wisey::IType* constructorType,
                                                int line) const {
  if (!constructorType->isFunction()) {
    context.reportError(line, "Injected pointer constructor must be a function");
    throw 1;
  }
  const LLVMFunctionType* functionType = ((const LLVMFunction*) constructorType)->getType();
  if (functionType->getReturnType() != mPointerType) {
    context.reportError(line, "Injected pointer constructor should return the pointer type");
    throw 1;
  }
  if (functionType->getArgumentTypes().size()) {
    context.reportError(line, "Injected pointer constructor should not have any arguments");
    throw 1;
  }
}

void LLVMPointerOwnerType::checkDestructorType(IRGenerationContext& context,
                                               const wisey::IType* destructorType,
                                               int line) const {
  if (!destructorType->isFunction()) {
    context.reportError(line, "Injected pointer destructor must be a function");
    throw 1;
  }
  const LLVMFunctionType* functionType = ((const LLVMFunction*) destructorType)->getType();
  if (functionType->getReturnType() != LLVMPrimitiveTypes::VOID) {
    context.reportError(line, "Injected pointer destructor should return ::llvm::void");
    throw 1;
  }
  if (functionType->getArgumentTypes().size() != 1) {
    context.reportError(line, "Injected pointer destructor function should have one argument");
    throw 1;
  }
  if (functionType->getArgumentTypes().front() != mPointerType) {
    context.reportError(line, "Injected pointer destructor should take pointer type "
                        "as the only argument");
    throw 1;
  }
}
