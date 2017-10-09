//
//  InterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Interface* InterfaceOwner::getObject() const {
  return mInterface;
}

string InterfaceOwner::getName() const {
  return mInterface->getName() + '*';
}

PointerType* InterfaceOwner::getLLVMType(LLVMContext& llvmContext) const {
  return (PointerType*) mInterface->getLLVMType(llvmContext)->getPointerElementType();
}

TypeKind InterfaceOwner::getTypeKind() const {
  return INTERFACE_OWNER_TYPE;
}

bool InterfaceOwner::canCastTo(const IType* toType) const {
  if (toType == this || toType == mInterface) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
    return mInterface->canCastTo(((IObjectOwnerType*) toType)->getObject());
  }
  
  return mInterface->canCastTo(toType);
}

bool InterfaceOwner::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* InterfaceOwner::castTo(IRGenerationContext& context,
                              Value* fromValue,
                              const IType* toType) const {
  if (toType == this || toType == mInterface) {
    return fromValue;
  }

  if (IType::isOwnerType(toType)) {
    return mInterface->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject());
  }
  
  return mInterface->castTo(context, fromValue, toType);
}

void InterfaceOwner::free(IRGenerationContext &context, Value *value) const {
  Value* thisPointer = Interface::getOriginalObject(context, value);
  
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(getLLVMType(llvmContext)->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, thisPointer, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 2);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* function = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  Type* argumentType = getLLVMType(llvmContext);
  Value* bitcast = IRWriter::newBitCastInst(context, thisPointer, argumentType);
  Value* bitcastStore = IRWriter::newAllocaInst(context, bitcast->getType(), "");
  IRWriter::newStoreInst(context, bitcast, bitcastStore);
  
  vector<Value*> arguments;
  arguments.push_back(bitcastStore);
  
  IRWriter::createCallInst(context, function, arguments, "");
}
