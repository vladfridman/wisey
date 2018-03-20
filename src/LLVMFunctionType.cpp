//
//  LLVMFunctionType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunctionType.hpp"
#include "wisey/ObjectFunctionVariable.hpp"

using namespace std;
using namespace wisey;

LLVMFunctionType::LLVMFunctionType(const ILLVMType* returnType,
                                   std::vector<const ILLVMType*> argumentTypes) :
mReturnType(returnType), mArgumentTypes(argumentTypes) {
  mPointerType = new LLVMPointerType(this);
}

LLVMFunctionType::~LLVMFunctionType() {
  delete mPointerType;
}

vector<const ILLVMType*> LLVMFunctionType::getArgumentTypes() const {
  return mArgumentTypes;
}

const ILLVMType* LLVMFunctionType::getReturnType() const {
  return mReturnType;
}

string LLVMFunctionType::getTypeName() const {
  string name = mReturnType->getTypeName();
  name = name + " (";
  for (const ILLVMType* llvmType : mArgumentTypes) {
    name = name + llvmType->getTypeName();
    if (llvmType != mArgumentTypes.back()) {
      name = name + ", ";
    }
  }
  name = name + ")";
  return name;
}

llvm::FunctionType* LLVMFunctionType::getLLVMType(IRGenerationContext& context) const {
  llvm::Type* llvmReturnType = mReturnType->getLLVMType(context);
  vector<llvm::Type*> argTypesArray;
  for (const ILLVMType* argumentType : mArgumentTypes) {
    argTypesArray.push_back(argumentType->getLLVMType(context));
  }
  return llvm::FunctionType::get(llvmReturnType, argTypesArray, false);
}

bool LLVMFunctionType::canCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

bool LLVMFunctionType::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return toType == this;
}

llvm::Value* LLVMFunctionType::castTo(IRGenerationContext &context,
                                   llvm::Value* fromValue,
                                   const IType* toType,
                                   int line) const {
  if (toType == this) {
    return fromValue;
  }
  
  return NULL;
}

bool LLVMFunctionType::isPrimitive() const {
  return false;
}

bool LLVMFunctionType::isOwner() const {
  return false;
}

bool LLVMFunctionType::isReference() const {
  return false;
}

bool LLVMFunctionType::isArray() const {
  return false;
}

bool LLVMFunctionType::isFunction() const {
  return true;
}

bool LLVMFunctionType::isPackage() const {
  return false;
}

bool LLVMFunctionType::isController() const {
  return false;
}

bool LLVMFunctionType::isInterface() const {
  return false;
}

bool LLVMFunctionType::isModel() const {
  return false;
}

bool LLVMFunctionType::isNode() const {
  return false;
}

bool LLVMFunctionType::isThread() const {
  return false;
}

bool LLVMFunctionType::isNative() const {
  return true;
}

void LLVMFunctionType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMFunctionType::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMFunctionType::createFieldVariable(IRGenerationContext& context,
                                           string name,
                                           const IConcreteObjectType* object) const {
  IVariable* variable = new ObjectFunctionVariable(name, object);
  context.getScopes().setVariable(variable);
}

void LLVMFunctionType::createParameterVariable(IRGenerationContext& context,
                                            string name,
                                            llvm::Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMFunctionType::getArrayType(IRGenerationContext& context) const {
  assert(false);
}

const IObjectType* LLVMFunctionType::getObjectType() const {
  return NULL;
}

const LLVMPointerType* LLVMFunctionType::getPointerType() const {
  return mPointerType;
}

const ILLVMType* LLVMFunctionType::getDereferenceType() const {
  assert(false);
}

