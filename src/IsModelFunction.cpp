//
//  IsModelFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IsModelFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* IsModelFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

Value* IsModelFunction::call(IRGenerationContext& context, Value* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* castObject = object->getType() != int8PointerType
  ? IRWriter::newBitCastInst(context, object, int8PointerType)
  : object;
  
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(castObject);
  
  return IRWriter::createCallInst(context, function, arguments, "");
}

string IsModelFunction::getName() {
  return "__isModel";
}

Function* IsModelFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* IsModelFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType());
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::I1, argumentTypes);
}

void IsModelFunction::compose(IRGenerationContext& context, llvm::Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* object = &*llvmArguments;
  object->setName("object");
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  
  context.setBasicBlock(entryBlock);
  Value* original = GetOriginalObjectFunction::call(context, object);
  Type* int8DoublePointerType = Type::getInt8Ty(llvmContext)->getPointerTo()->getPointerTo();
  Type* int8TriplePointerType = int8DoublePointerType->getPointerTo();
  Value* vTablePointer = IRWriter::newBitCastInst(context, original, int8TriplePointerType);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 1);
  GetElementPtrInst* typeArrayPointerI8 = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* typeArrayI8 = IRWriter::newLoadInst(context, typeArrayPointerI8, "typeArrayI8");
  BitCastInst* arrayOfStrings = IRWriter::newBitCastInst(context,
                                                         typeArrayI8,
                                                         int8DoublePointerType);
  LoadInst* stringPointer = IRWriter::newLoadInst(context, arrayOfStrings, "stringPointer");
  
  Value* firstLetter = IRWriter::newLoadInst(context, stringPointer, "firstLetter");
  
  Value* letterM = ConstantInt::get(Type::getInt8Ty(llvmContext), 77);
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, firstLetter, letterM, "");
  IRWriter::createReturnInst(context, condition);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
