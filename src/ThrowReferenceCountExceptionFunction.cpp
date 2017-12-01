//
//  ThrowReferenceCountExceptionFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
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

string ThrowReferenceCountExceptionFunction::getName() {
  return "__throwReferenceCountException";
}

Function* ThrowReferenceCountExceptionFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(functionType,
                          GlobalValue::InternalLinkage,
                          getName(),
                          context.getModule());
}

void ThrowReferenceCountExceptionFunction::compose(IRGenerationContext& context,
                                                llvm::Function* function) {
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("referenceCount");
  Value* referenceCount = llvmArgument;

  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(Names::getLangPackageName(), Names::getReferenceCountExceptionName());
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  FakeExpression* fakeExpression = new FakeExpression(referenceCount, PrimitiveTypes::LONG_TYPE);
  ObjectBuilderArgument* argument = new ObjectBuilderArgument("withReferenceCount", fakeExpression);
  objectBuilderArgumnetList.push_back(argument);
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier, objectBuilderArgumnetList);
  ThrowStatement throwStatement(objectBuilder, 0);
  
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  IRWriter::createReturnInst(context, NULL);
}
