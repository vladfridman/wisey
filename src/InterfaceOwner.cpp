//
//  InterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceOwner::InterfaceOwner(Interface* interface) : mInterface(interface) { }

InterfaceOwner::~InterfaceOwner() { }

Interface* InterfaceOwner::getObject() const {
  return mInterface;
}

string InterfaceOwner::getTypeName() const {
  return mInterface->getTypeName() + '*';
}

PointerType* InterfaceOwner::getLLVMType(IRGenerationContext& context) const {
  return mInterface->getLLVMType(context);
}

TypeKind InterfaceOwner::getTypeKind() const {
  return INTERFACE_OWNER_TYPE;
}

bool InterfaceOwner::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mInterface) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
    return mInterface->canCastTo(context, ((IObjectOwnerType*) toType)->getObject());
  }
  
  return mInterface->canCastTo(context, toType);
}

bool InterfaceOwner::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* InterfaceOwner::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType,
                              int line) const {
  if (toType == this || toType == mInterface) {
    return fromValue;
  }

  if (IType::isOwnerType(toType)) {
    return mInterface->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject(), line);
  }
  
  return mInterface->castTo(context, fromValue, toType, line);
}

void InterfaceOwner::free(IRGenerationContext& context, Value* value) const {
  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8pointer);

  Function* destructor = getDestructorFunction(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);

  IRWriter::createCallInst(context, destructor, arguments, "");
}

Function* InterfaceOwner::getDestructorFunction(IRGenerationContext& context) const {
  Function* destructor = context.getModule()->getFunction(mInterface->getDestructorFunctionName());
  
  return destructor ? destructor : mInterface->defineDestructorFunction(context);
}

bool InterfaceOwner::isOwner() const {
  return true;
}

bool InterfaceOwner::isReference() const {
  return false;
}

bool InterfaceOwner::isArray() const {
  return false;
}

void InterfaceOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void InterfaceOwner::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void InterfaceOwner::createFieldVariable(IRGenerationContext& context,
                                          string name,
                                          const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void InterfaceOwner::createParameterVariable(IRGenerationContext& context,
                                             string name,
                                             Value* value) const {
  Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, "parameterObjectPointer");
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* InterfaceOwner::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

