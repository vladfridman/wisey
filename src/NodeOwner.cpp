//
//  NodeOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "FieldOwnerVariable.hpp"
#include "IRWriter.hpp"
#include "NodeOwner.hpp"
#include "LocalOwnerVariable.hpp"
#include "ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NodeOwner::NodeOwner(Node* node) : mNode(node) { }

NodeOwner::~NodeOwner() {
}

Node* NodeOwner::getReference() const {
  return mNode;
}

string NodeOwner::getTypeName() const {
  return mNode->getTypeName() + '*';
}

llvm::PointerType* NodeOwner::getLLVMType(IRGenerationContext& context) const {
  return mNode->getLLVMType(context);
}

bool NodeOwner::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mNode) {
    return true;
  }
  
  if (toType->isArray()) {
    return false;
  }
  
  if (toType->isOwner()) {
    return mNode->canCastTo(context, ((const IOwnerType*) toType)->getReference());
  }
  
  return mNode->canCastTo(context, toType);
}

bool NodeOwner::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* NodeOwner::castTo(IRGenerationContext& context,
                         Value* fromValue,
                         const IType* toType,
                         int line) const {
  if (toType == this || toType == mNode) {
    return fromValue;
  }
  
  
  if (toType->isOwner()) {
    const IReferenceType* toReferenceType = ((const IOwnerType*) toType)->getReference();
    return mNode->castTo(context, fromValue, toReferenceType, line);
  }
  
  return mNode->castTo(context, fromValue, toType, line);
}

void NodeOwner::free(IRGenerationContext &context,
                     Value* value,
                     Value* exception,
                     const LLVMFunction* customDestructor,
                     int line) const {
  IConcreteObjectType::composeDestructorCall(context, mNode, value, exception, line);
}

bool NodeOwner::isPrimitive() const {
  return false;
}

bool NodeOwner::isOwner() const {
  return true;
}

bool NodeOwner::isReference() const {
  return false;
}

bool NodeOwner::isArray() const {
  return false;
}

bool NodeOwner::isFunction() const {
  return false;
}

bool NodeOwner::isPackage() const {
  return false;
}

bool NodeOwner::isController() const {
  return false;
}

bool NodeOwner::isInterface() const {
  return false;
}

bool NodeOwner::isModel() const {
  return false;
}

bool NodeOwner::isNode() const {
  return true;
}

bool NodeOwner::isNative() const {
  return false;
}

bool NodeOwner::isPointer() const {
  return false;
}

bool NodeOwner::isImmutable() const {
  return false;
}

void NodeOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void NodeOwner::createLocalVariable(IRGenerationContext& context,
                                    string name,
                                    int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, uninitializedVariable);
}

void NodeOwner::createFieldVariable(IRGenerationContext& context,
                                    string name,
                                    const IConcreteObjectType* object,
                                    int line) const {
  IVariable* variable = new FieldOwnerVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void NodeOwner::createParameterVariable(IRGenerationContext& context,
                                        string name,
                                        Value* value,
                                        int line) const {
  Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc, line);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* NodeOwner::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* NodeOwner::inject(IRGenerationContext& context,
                               const InjectionArgumentList injectionArgumentList,
                               int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}
