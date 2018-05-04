//
//  DestroyObjectOwnerFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/30/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyObjectOwnerFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunctionType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyObjectOwnerFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyObjectOwnerFunction::call(IRGenerationContext& context, Value* objectReference) {
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(objectReference);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string DestroyObjectOwnerFunction::getName() {
  return "__destroyObjectOwnerFunction";
}

Function* DestroyObjectOwnerFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* DestroyObjectOwnerFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void DestroyObjectOwnerFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  context.getScopes().pushScope();
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Value* thisGeneric = &*functionArguments;
  thisGeneric->setName("thisGeneric");
  functionArguments++;
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  
  context.setBasicBlock(entryBlock);
  
  Value* nullValue = ConstantPointerNull::get(int8Type->getPointerTo());
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisGeneric, nullValue, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  
  Value* originalObjectVTable = GetOriginalObjectFunction::call(context, thisGeneric);
  
  Type* pointerToVTablePointer = function->getFunctionType()
  ->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 2);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* objectDestructor = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  vector<Value*> arguments;
  arguments.push_back(originalObjectVTable);
  
  IRWriter::createInvokeInst(context, objectDestructor, arguments, "", 0);
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
