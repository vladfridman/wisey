//
//  GetOriginalObjectFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* GetOriginalObjectFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

Value* GetOriginalObjectFunction::call(IRGenerationContext& context, Value* interfacePointer) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, interfacePointer, int8PointerType);
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  return IRWriter::createCallInst(context, function, arguments, "");
}

string GetOriginalObjectFunction::getName() {
  return "__getOriginalObject";
}

Function* GetOriginalObjectFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* GetOriginalObjectFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType());
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::I8->getPointerType(), argumentTypes);
}

void GetOriginalObjectFunction::compose(IRGenerationContext& context, Function* function) {
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* interfacePointer = &*llvmArguments;
  interfacePointer->setName("pointer");
  
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);

  Value* unthunkBy = getUnthunkBy(context, interfacePointer);
  
  Value* index[1];
  index[0] = unthunkBy;
  Value* originalObject = IRWriter::createGetElementPtrInst(context, interfacePointer, index);
  
  IRWriter::createReturnInst(context, originalObject);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context));
}

Value* GetOriginalObjectFunction::getUnthunkBy(IRGenerationContext& context, Value* value) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer = IRWriter::newBitCastInst(context, value, pointerType);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 0);
  GetElementPtrInst* unthunkPointer = IRWriter::createGetElementPtrInst(context, vTable, index);
  
  LoadInst* pointerToVal = IRWriter::newLoadInst(context, unthunkPointer, "unthunkbypointer");
  Value* unthunkBy = IRWriter::newPtrToIntInst(context,
                                               pointerToVal,
                                               Type::getInt64Ty(llvmContext),
                                               "unthunkby");
  return unthunkBy;
}

