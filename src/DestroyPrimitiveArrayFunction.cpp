//
//  DestroyPrimitiveArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/CheckArrayNotReferencedFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
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

void DestroyPrimitiveArrayFunction::call(IRGenerationContext& context,
                                         Value* array,
                                         unsigned long numberOfDimensions) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(array);
  arguments.push_back(ConstantInt::get(Type::getInt64Ty(llvmContext), numberOfDimensions));

  IRWriter::createCallInst(context, function, arguments, "");
}

string DestroyPrimitiveArrayFunction::getName() {
  return "__destroyPrimitiveArrayFunction";
}

Function* DestroyPrimitiveArrayFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* DestroyPrimitiveArrayFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I64->getPointerType());
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);

  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void DestroyPrimitiveArrayFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int64type = Type::getInt64Ty(llvmContext);
  llvm::PointerType* genericPointer = int64type->getPointerTo();

  context.getScopes().pushScope();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  Value* arrayPointer = &*llvmArguments;
  arrayPointer->setName("arrayPointer");
  llvmArguments++;
  Value* numberOfDimensions = &*llvmArguments;
  numberOfDimensions->setName("noOfDimensions");
  
  BasicBlock* entry = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* returnVoid = BasicBlock::Create(llvmContext, "return.void", function);
  BasicBlock* ifNotNull = BasicBlock::Create(llvmContext, "if.not.null", function);
  
  context.setBasicBlock(entry);
  Value* null = ConstantPointerNull::get(genericPointer);
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, arrayPointer, null, "isNull");
  IRWriter::createConditionalBranch(context, returnVoid, ifNotNull, isNull);
  
  context.setBasicBlock(returnVoid);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNull);
  CheckArrayNotReferencedFunction::call(context, arrayPointer, numberOfDimensions);
  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    printOutArguments.push_back(new StringLiteral("destructor primitive[]\n", 0));
    PrintOutStatement printOutStatement(printOutArguments);
    printOutStatement.generateIR(context);
  }
  IRWriter::createFree(context, arrayPointer);
  IRWriter::createReturnInst(context, NULL);

  context.getScopes().popScope(context, 0);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context));
}
