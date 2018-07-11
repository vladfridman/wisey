//
//  ThrowNullPointerExceptionFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThrowNullPointerExceptionFunction.hpp"
#include "wisey/ThrowStatement.hpp"

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
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement throwStatement(objectBuilder, 0);
  
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  IRWriter::createReturnInst(context, NULL);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
