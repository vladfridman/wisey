//
//  DestroyPrimitiveArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyPrimitiveArrayFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyPrimitiveArrayFunction::call(IRGenerationContext& context, Value* array) {
  Function* function = DestroyPrimitiveArrayFunction::get(context);
  vector<Value*> arguments;
  arguments.push_back(array);
 
  IRWriter::createCallInst(context, function, arguments, "");
}

string DestroyPrimitiveArrayFunction::getName() {
  return "__destroyPrimitiveArrayFunction";
}

Function* DestroyPrimitiveArrayFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  PointerType* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();

  vector<Type*> argumentTypes;
  argumentTypes.push_back(genericPointer);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void DestroyPrimitiveArrayFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  PointerType* genericPointer = llvm::Type::getInt64Ty(context.getLLVMContext())->getPointerTo();

  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("arrayPointer");
  Value* arrayPointer = llvmArgument;
  
  llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvmContext, "entry", function);
  llvm::BasicBlock* ifNull = llvm::BasicBlock::Create(llvmContext, "if.null", function);
  llvm::BasicBlock* freeArray = llvm::BasicBlock::Create(llvmContext, "free.array", function);
  
  context.setBasicBlock(entry);
  
  Value* null = ConstantPointerNull::get(genericPointer);
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, arrayPointer, null, "");
  IRWriter::createConditionalBranch(context, ifNull, freeArray, isNull);
  
  context.setBasicBlock(ifNull);
  
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(freeArray);
  
  if (context.isDestructorDebugOn()) {
    vector<IExpression*> printOutArguments;
    printOutArguments.push_back(new StringLiteral("destructor primitive[]\n"));
    PrintOutStatement printOutStatement(printOutArguments);
    printOutStatement.generateIR(context);
  }

  IRWriter::createFree(context, arrayPointer);
  IRWriter::createReturnInst(context, NULL);
}
