//
//  ThrowNullPointerExceptionFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "Composer.hpp"
#include "FakeExpression.hpp"
#include "IRWriter.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "ModelTypeSpecifier.hpp"
#include "Names.hpp"
#include "HeapBuilder.hpp"
#include "PrimitiveTypes.hpp"
#include "ThrowNullPointerExceptionFunction.hpp"
#include "ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* ThrowNullPointerExceptionFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void ThrowNullPointerExceptionFunction::call(IRGenerationContext& context) {
  Function* function = get(context);
  vector<Value*> arguments;
  
  IRWriter::createInvokeInst(context, function, arguments, "", 0);
}

string ThrowNullPointerExceptionFunction::getName() {
  return "__throwNPE";
}

Function* ThrowNullPointerExceptionFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* ThrowNullPointerExceptionFunction::getLLVMFunctionType(IRGenerationContext&
                                                                         context) {
  vector<const IType*> argumentTypes;
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void ThrowNullPointerExceptionFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function);
  context.setBasicBlock(basicBlock);
  
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(packageExpression,
                                                                  Names::getNPEModelName(),
                                                                  0);
  BuilderArgumentList builderArgumnetList;
  HeapBuilder* heapBuilder = new HeapBuilder(modelTypeSpecifier,
                                                   builderArgumnetList,
                                                   0);
  ThrowStatement throwStatement(heapBuilder, 0);
  
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  IRWriter::createReturnInst(context, NULL);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
