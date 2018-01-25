//
//  DestroyPrimitiveArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

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
  Function* function = get(context);
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

  context.getScopes().pushScope();

  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("arrayPointer");
  Value* arrayPointer = llvmArgument;
  
  BasicBlock* entry = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNull = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNull = BasicBlock::Create(llvmContext, "if.not.null", function);
  BasicBlock* refCountZeroBlock = BasicBlock::Create(llvmContext, "ref.count.zero", function);
  BasicBlock* refCountNotZeroBlock = BasicBlock::Create(llvmContext, "ref.count.notzero", function);
  
  context.setBasicBlock(entry);
  
  Value* null = ConstantPointerNull::get(genericPointer);
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, arrayPointer, null, "");
  IRWriter::createConditionalBranch(context, ifNull, ifNotNull, isNull);
  
  context.setBasicBlock(ifNull);
  
  IRWriter::createReturnInst(context, NULL);

  context.setBasicBlock(ifNotNull);

  llvm::Type* int64type = llvm::Type::getInt64Ty(llvmContext);
  Value* index[1];
  index[0] = ConstantInt::get(int64type, 2);
  Value* sizeStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* size = IRWriter::newLoadInst(context, sizeStore, "size");

  if (context.isDestructorDebugOn()) {
    vector<IExpression*> printOutArguments;
    printOutArguments.push_back(new StringLiteral("destructor primitive["));
    printOutArguments.push_back(new FakeExpression(size, PrimitiveTypes::INT_TYPE));
    printOutArguments.push_back(new StringLiteral("]\n"));
    PrintOutStatement printOutStatement(printOutArguments);
    printOutStatement.generateIR(context);
  }

  Value* referenceCount = IRWriter::newLoadInst(context, arrayPointer, "");
  llvm::Constant* zero = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, referenceCount, zero, "");
  IRWriter::createConditionalBranch(context, refCountZeroBlock, refCountNotZeroBlock, isZero);
  
  context.setBasicBlock(refCountNotZeroBlock);
  
  ThrowReferenceCountExceptionFunction::call(context, referenceCount);
  IRWriter::newUnreachableInst(context);

  context.setBasicBlock(refCountZeroBlock);

  IRWriter::createFree(context, arrayPointer);
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);
}
