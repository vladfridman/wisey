//
//  InterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceOwner::InterfaceOwner(Interface* interface) : mInterface(interface) { }

InterfaceOwner::~InterfaceOwner() { }

Interface* InterfaceOwner::getObject() const {
  return mInterface;
}

string InterfaceOwner::getName() const {
  return mInterface->getName() + '*';
}

PointerType* InterfaceOwner::getLLVMType(LLVMContext& llvmContext) const {
  return mInterface->getLLVMType(llvmContext);
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
  argumentTypes.push_back(getLLVMType(llvmContext));
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

  Value* nullValue = ConstantPointerNull::get(getLLVMType(llvmContext));
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisArgument, nullValue, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);

  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);

  Value* originalObjectVTable = Interface::getOriginalObjectVTable(context, thisArgument);
  
  Type* pointerToVTablePointer = functionType->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 2);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* objectDestructor = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  ConstantInt* bytes = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                        -Environment::getAddressSizeInBytes());
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* pointerToVTable = IRWriter::newBitCastInst(context,
                                                          originalObjectVTable,
                                                          int8Type->getPointerTo());
  index[0] = bytes;
  Value* thisPointer = IRWriter::createGetElementPtrInst(context, pointerToVTable, index);
  
  Type* argumentType = getLLVMType(llvmContext);
  Value* bitcast = IRWriter::newBitCastInst(context, thisPointer, argumentType);
  
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  IRWriter::createCallInst(context, objectDestructor, arguments, "");
  IRWriter::createReturnInst(context, NULL);

  context.setBasicBlock(lastBasicBlock);
  
  return destructor;

}

