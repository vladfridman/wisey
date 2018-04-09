//
//  ThrowReferenceCountExceptionFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/ObjectBuilderArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* ThrowReferenceCountExceptionFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);

  return function;
}

void ThrowReferenceCountExceptionFunction::call(IRGenerationContext& context,
                                                Value* referenceCount) {
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(referenceCount);
  
  IRWriter::createInvokeInst(context, function, arguments, "", 0);
}

string ThrowReferenceCountExceptionFunction::getName() {
  return "__throwReferenceCountException";
}

Function* ThrowReferenceCountExceptionFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* ThrowReferenceCountExceptionFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void ThrowReferenceCountExceptionFunction::compose(IRGenerationContext& context,
                                                llvm::Function* function) {
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("referenceCount");
  Value* referenceCount = llvmArgument;

  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(packageExpression, Names::getReferenceCountExceptionName(), 0);
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  FakeExpression* fakeExpression = new FakeExpression(referenceCount, PrimitiveTypes::LONG_TYPE);
  ObjectBuilderArgument* argument = new ObjectBuilderArgument("withReferenceCount", fakeExpression);
  objectBuilderArgumnetList.push_back(argument);
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement throwStatement(objectBuilder, 0);
  
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  IRWriter::createReturnInst(context, NULL);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context));
}
