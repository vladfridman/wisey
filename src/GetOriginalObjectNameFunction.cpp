//
//  GetOriginalObjectNameFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* GetOriginalObjectNameFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

Value* GetOriginalObjectNameFunction::call(IRGenerationContext& context, Value* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* bitcast = object->getType() != int8PointerType
  ? IRWriter::newBitCastInst(context, object, int8PointerType)
  : object;
  
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  return IRWriter::createCallInst(context, function, arguments, "");
}

string GetOriginalObjectNameFunction::getName() {
  return "__getOriginalObjectName";
}

Function* GetOriginalObjectNameFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* GetOriginalObjectNameFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::I8->getPointerType(context, 0),
                                     argumentTypes);
}

void GetOriginalObjectNameFunction::compose(IRGenerationContext& context, Function* function) {
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* object = &*llvmArguments;
  object->setName("object");
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);

  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  Value* originalObjectVTable = GetOriginalObjectFunction::call(context, object);
  Type* pointerToArrayOfStrings = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToArrayOfStrings);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 1);
  GetElementPtrInst* typeArrayPointerI8 = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* typeArrayI8 = IRWriter::newLoadInst(context, typeArrayPointerI8, "typeArrayI8");
  BitCastInst* arrayOfStrings =
  IRWriter::newBitCastInst(context, typeArrayI8, int8Type->getPointerTo()->getPointerTo());
  Value* namePointer = IRWriter::createGetElementPtrInst(context, arrayOfStrings, index);
  LoadInst* fromTypeName = IRWriter::newLoadInst(context, namePointer, "name");

  IRWriter::createReturnInst(context, fromTypeName, 0);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
