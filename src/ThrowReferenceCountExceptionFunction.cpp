//
//  ThrowReferenceCountExceptionFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "FakeExpression.hpp"
#include "IRWriter.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "ModelTypeSpecifier.hpp"
#include "Names.hpp"
#include "HeapBuilder.hpp"
#include "BuilderArgument.hpp"
#include "PrimitiveTypes.hpp"
#include "ThrowReferenceCountExceptionFunction.hpp"
#include "ThrowStatement.hpp"

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
                                                Value* referenceCount,
                                                llvm::Value* namePointer,
                                                llvm::Value* exception) {
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(referenceCount);
  arguments.push_back(namePointer);
  arguments.push_back(exception);

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
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));

  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void ThrowReferenceCountExceptionFunction::compose(IRGenerationContext& context,
                                                   llvm::Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* referenceCount = &*llvmArguments;
  referenceCount->setName("referenceCount");
  llvmArguments++;
  llvm::Argument* namePointer = &*llvmArguments;
  namePointer->setName("namePointer");
  llvmArguments++;
  llvm::Argument* exception = &*llvmArguments;
  exception->setName("exception");

  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);

  context.setBasicBlock(entryBlock);
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(packageExpression, Names::getReferenceCountExceptionName(), 0);
  BuilderArgumentList builderArgumnetList;
  FakeExpression* fakeExpression = new FakeExpression(referenceCount, PrimitiveTypes::LONG);
  BuilderArgument* refCountArgument = new BuilderArgument("withReferenceCount", fakeExpression);
  fakeExpression = new FakeExpression(namePointer, PrimitiveTypes::STRING);
  BuilderArgument* objectNameArgument = new BuilderArgument("withObjectType", fakeExpression);
  Interface* exceptionInterface = context.getInterface(Names::getExceptionInterfaceFullName(), 0);
  Value* exceptionCast = IRWriter::newBitCastInst(context,
                                                  exception,
                                                  exceptionInterface->getLLVMType(context));
  fakeExpression = new FakeExpression(exceptionCast, exceptionInterface);
  BuilderArgument* nestedExceptionArgument = new BuilderArgument("withNestedException", fakeExpression);
  builderArgumnetList.push_back(refCountArgument);
  builderArgumnetList.push_back(objectNameArgument);
  builderArgumnetList.push_back(nestedExceptionArgument);
  HeapBuilder* heapBuilder = new HeapBuilder(modelTypeSpecifier, builderArgumnetList, 0);
  ThrowStatement throwStatement(heapBuilder, 0);
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  IRWriter::createReturnInst(context, NULL);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
