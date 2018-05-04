//
//  DestroyNativeObjectFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyNativeObjectFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunctionType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyNativeObjectFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyNativeObjectFunction::call(IRGenerationContext& context, Value* reference, int line) {
  Function* function = get(context);
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Value*> arguments;
  if (reference->getType() == int8PointerType) {
    arguments.push_back(reference);
  } else {
    Value* genericReference = IRWriter::newBitCastInst(context, reference, int8PointerType);
    arguments.push_back(genericReference);
  }
  
  IRWriter::createCallInst(context, function, arguments, "", line);
}

string DestroyNativeObjectFunction::getName() {
  return "__destroyNativeObjectFunction";
}

Function* DestroyNativeObjectFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* DestroyNativeObjectFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void DestroyNativeObjectFunction::compose(IRGenerationContext& context, Function* function) {
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
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition, 0);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL, 0);
  
  context.setBasicBlock(ifNotNullBlock);
  
  IRWriter::createFree(context, thisGeneric, 0);

  IRWriter::createReturnInst(context, NULL, 0);
  
  context.getScopes().popScope(context, 0);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
