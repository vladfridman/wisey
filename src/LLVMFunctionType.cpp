//
//  LLVMFunctionType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "IRGenerationContext.hpp"
#include "LLVMFunctionType.hpp"

using namespace std;
using namespace wisey;

LLVMFunctionType::LLVMFunctionType(const IType* returnType,
                                   std::vector<const IType*> argumentTypes,
                                   bool isVarArg) :
mReturnType(returnType), mArgumentTypes(argumentTypes), mIsVarArg(isVarArg) {
  mPointerType = LLVMPointerType::create(this);
}

LLVMFunctionType::~LLVMFunctionType() {
  delete mPointerType;
}

LLVMFunctionType* LLVMFunctionType::create(const IType* returnType,
                                           std::vector<const IType*> argumentTypes) {
  return new LLVMFunctionType(returnType, argumentTypes, false);
}

LLVMFunctionType* LLVMFunctionType::createWithVarArg(const IType* returnType,
                                                     std::vector<const IType*> argumentTypes) {
  return new LLVMFunctionType(returnType, argumentTypes, true);
}

vector<const IType*> LLVMFunctionType::getArgumentTypes() const {
  return mArgumentTypes;
}

const IType* LLVMFunctionType::getReturnType() const {
  return mReturnType;
}

bool LLVMFunctionType::isVarArg() const {
  return mIsVarArg;
}

string LLVMFunctionType::getTypeName() const {
  string name = mReturnType->getTypeName();
  name = name + " (";
  for (const IType* llvmType : mArgumentTypes) {
    name = name + llvmType->getTypeName();
    if (llvmType != mArgumentTypes.back()) {
      name = name + ", ";
    }
  }
  if (mIsVarArg) {
    if (mArgumentTypes.size()) {
      name = name + ", ...";
    } else {
      name = name + "...";
    }
  }
  name = name + ")";
  return name;
}

llvm::FunctionType* LLVMFunctionType::getLLVMType(IRGenerationContext& context) const {
  llvm::Type* llvmReturnType = mReturnType->getLLVMType(context);
  vector<llvm::Type*> argTypesArray;
  for (const IType* argumentType : mArgumentTypes) {
    argTypesArray.push_back(argumentType->getLLVMType(context));
  }
  return llvm::FunctionType::get(llvmReturnType, argTypesArray, mIsVarArg);
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

bool LLVMFunctionType::isNative() const {
  return true;
}

bool LLVMFunctionType::isPointer() const {
  return false;
}

bool LLVMFunctionType::isImmutable() const {
  return false;
}

void LLVMFunctionType::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << getTypeName();
}

void LLVMFunctionType::createLocalVariable(IRGenerationContext& context,
                                           string name,
                                           int line) const {
  assert(false);
}

void LLVMFunctionType::createFieldVariable(IRGenerationContext& context,
                                           string name,
                                           const IConcreteObjectType* object,
                                           int line) const {
  assert(false);
}

void LLVMFunctionType::createParameterVariable(IRGenerationContext& context,
                                            string name,
                                               llvm::Value* value,
                                               int line) const {
  assert(false);
}

const wisey::ArrayType* LLVMFunctionType::getArrayType(IRGenerationContext& context,
                                                       int line) const {
  assert(false);
}

const LLVMPointerType* LLVMFunctionType::getPointerType(IRGenerationContext& context,
                                                        int line) const {
  return mPointerType;
}

llvm::Instruction* LLVMFunctionType::inject(IRGenerationContext& context,
                                            const InjectionArgumentList injectionArgumentList,
                                            int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
