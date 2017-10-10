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

void InterfaceOwner::free(IRGenerationContext& context, Value* value) const {
  Function* destructor = context.getModule()->getFunction(getDestructorFunctionName());
  
  if (destructor == NULL) {
    destructor = composeDestructorFunction(context);
  }

  vector<Value*> arguments;
  arguments.push_back(value);

  IRWriter::createCallInst(context, destructor, arguments, "");
}

string InterfaceOwner::getDestructorFunctionName() const {
  return "destructor." + mInterface->getName();
}

Function* InterfaceOwner::composeDestructorFunction(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  vector<Type*> argumentTypes;
  argumentTypes.push_back(getLLVMType(llvmContext)->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* voidType = Type::getVoidTy(llvmContext);
  FunctionType* functionType = FunctionType::get(voidType, argTypesArray, false);
  Function* destructor = Function::Create(functionType,
                                          GlobalValue::ExternalLinkage,
                                          getDestructorFunctionName(),
                                          context.getModule());

  Function::arg_iterator functionArguments = destructor->arg_begin();
  Argument* thisArgument = &*functionArguments;
  thisArgument->setName("this");
  functionArguments++;
  
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", destructor);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", destructor);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", destructor);
  
  context.setBasicBlock(entryBlock);

  Value* thisLoaded = IRWriter::newLoadInst(context, thisArgument, "");
  Value* nullValue = ConstantPointerNull::get(getLLVMType(llvmContext));
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisLoaded, nullValue, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);

  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);

  Value* thisPointer = Interface::getOriginalObject(context, thisArgument);
  
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, thisPointer, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 2);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* objectDestructor = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  Type* argumentType = getLLVMType(llvmContext);
  Value* bitcast = IRWriter::newBitCastInst(context, thisPointer, argumentType);
  Value* bitcastStore = IRWriter::newAllocaInst(context, bitcast->getType(), "");
  IRWriter::newStoreInst(context, bitcast, bitcastStore);
  
  vector<Value*> arguments;
  arguments.push_back(bitcastStore);
  
  IRWriter::createCallInst(context, objectDestructor, arguments, "");
  IRWriter::createReturnInst(context, NULL);

  context.setBasicBlock(lastBasicBlock);
  
  return destructor;

}

