//
//  DestroyOwnerObjectFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/30/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyOwnerObjectFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyOwnerObjectFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyOwnerObjectFunction::call(IRGenerationContext& context, Value* objectReference) {
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(objectReference);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string DestroyOwnerObjectFunction::getName() {
  return "__destroyOwnerObjectFunction";
}

Function* DestroyOwnerObjectFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(int8Pointer);
  Type* voidType = Type::getVoidTy(llvmContext);
  FunctionType* functionType = FunctionType::get(voidType, argumentTypes, false);
  return Function::Create(functionType,
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

void DestroyOwnerObjectFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  context.getScopes().pushScope();
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisGeneric = &*functionArguments;
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
  
  Value* originalObjectVTable = GetOriginalObjectFunction::callGetVTable(context, thisGeneric);
  
  Type* pointerToVTablePointer = function->getFunctionType()
  ->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 2);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* objectDestructor = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  ConstantInt* bytes = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                        -Environment::getAddressSizeInBytes());
  BitCastInst* pointerToVTable = IRWriter::newBitCastInst(context,
                                                          originalObjectVTable,
                                                          int8Type->getPointerTo());
  index[0] = bytes;
  Value* thisPointer = IRWriter::createGetElementPtrInst(context, pointerToVTable, index);
  
  vector<Value*> arguments;
  arguments.push_back(thisPointer);
  
  IRWriter::createInvokeInst(context, objectDestructor, arguments, "", 0);
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);
}
