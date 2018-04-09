//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForArrayFunction.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/CastExpression.hpp"
#include "wisey/CheckArrayIndexFunction.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyOwnerObjectFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/ExpressionStatement.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void ProgramSuffix::generateIR(IRGenerationContext& context) const {
  defineEssentialFunctions(context);
  maybeGenerateMain(context);
}

void ProgramSuffix::defineEssentialFunctions(IRGenerationContext& context) const {
  AdjustReferenceCounterForArrayFunction::get(context);
  AdjustReferenceCounterForConcreteObjectSafelyFunction::get(context);
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(context);
  AdjustReferenceCountFunction::get(context);
  CheckArrayIndexFunction::get(context);
  DestroyOwnerArrayFunction::get(context);
  DestroyOwnerObjectFunction::get(context);
  DestroyPrimitiveArrayFunction::get(context);
  DestroyReferenceArrayFunction::get(context);
  GetOriginalObjectFunction::get(context);
  ThrowReferenceCountExceptionFunction::get(context);
}

void ProgramSuffix::maybeGenerateMain(IRGenerationContext& context) const {
  PackageType* langPackageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* langPackageExpression = new FakeExpression(NULL, langPackageType);
  InterfaceTypeSpecifier* programInterfaceSpecifier =
  new InterfaceTypeSpecifier(langPackageExpression, Names::getIProgramName(), 0);
  Interface* interface = (Interface*) programInterfaceSpecifier->getType(context);
  if (!context.hasBoundController(interface)) {
    return;
  }
  
  generateMain(context);
}

void ProgramSuffix::generateMain(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  FunctionType* mainFunctionType =
  FunctionType::get(Type::getInt32Ty(llvmContext), false);
  Function* mainFunction = Function::Create(mainFunctionType,
                                            GlobalValue::ExternalLinkage,
                                            "main",
                                            context.getModule());
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", mainFunction);
  context.setBasicBlock(entryBlock);
  context.getScopes().pushScope();
  context.getScopes().setReturnType(PrimitiveTypes::INT_TYPE);
  
  InjectionArgumentList injectionArguments;
  Thread* mainThread = context.getThread(Names::getMainThreadFullName(), 0);
  Value* injectedThread = mainThread->inject(context, injectionArguments, 0);
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  PackageType* langPackageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* langPackageExpression = new FakeExpression(NULL, langPackageType);
  PackageType* threadsPackageType = context.getPackageType(Names::getThreadsPackageName());
  FakeExpression* threadsPackageExpression = new FakeExpression(NULL, threadsPackageType);
  InterfaceTypeSpecifier* threadInterfaceSpecifier =
    new InterfaceTypeSpecifier(threadsPackageExpression, Names::getThreadInterfaceName(), 0);
  CastExpression* castExpression =
  new CastExpression(threadInterfaceSpecifier, new FakeExpression(injectedThread, mainThread), 0);
  IReferenceVariable* threadVariable =
  new ParameterSystemReferenceVariable(ThreadExpression::THREAD,
                                       threadInterface,
                                       castExpression->generateIR(context, threadInterface));
  context.getScopes().setVariable(threadVariable);

  vector<const IExpression*> arrayIndices;
  Value* mainThreadStore = IRWriter::newAllocaInst(context,
                                                   mainThread->getOwner()->getLLVMType(context),
                                                   "mainThread");
  IOwnerVariable* mainThreadVariable = new LocalOwnerVariable("mainThread",
                                                              mainThread->getOwner(),
                                                              mainThreadStore);
  context.getScopes().setVariable(mainThreadVariable);
  mainThreadVariable->setToNull(context);
  FakeExpression mainThreadExpression(injectedThread, mainThread->getOwner());
  mainThreadVariable->generateAssignmentIR(context, &mainThreadExpression, arrayIndices, 0);

  Controller* callStack = context.getController(Names::getCallStackControllerFullName(), 0);
  IReferenceVariable* callStackVariable =
  new ParameterSystemReferenceVariable(ThreadExpression::CALL_STACK,
                                       callStack,
                                       ConstantPointerNull::get(callStack->getLLVMType(context)));
  context.getScopes().setVariable(callStackVariable);
  
  IdentifierChain* startMethod = new IdentifierChain(new Identifier("mainThread"), "start");
  ExpressionList callArguments;
  MethodCall* startMethodCall = new MethodCall(startMethod, callArguments, 0);
  ExpressionStatement startMethodCallStatement(startMethodCall);
  startMethodCallStatement.generateIR(context);
  
  IdentifierChain* waitMethod = new IdentifierChain(new Identifier("mainThread"), "awaitResult");
  MethodCall* waitMethodCall = new MethodCall(waitMethod, callArguments, 0);
  
  langPackageExpression = new FakeExpression(NULL, langPackageType);
  ModelTypeSpecifier* mainThreadMessageSpecifier =
  new ModelTypeSpecifier(langPackageExpression, Names::getProgramResultShortName(), 0);
  CastExpression* castToMessage = new CastExpression(mainThreadMessageSpecifier, waitMethodCall, 0);
  IdentifierChain* getContentMethod = new IdentifierChain(castToMessage, "getResult");
  MethodCall* getContent = new MethodCall(getContentMethod, callArguments, 0);
  ReturnStatement returnStatement(getContent, 0);
  returnStatement.generateIR(context);

  context.getScopes().popScope(context, 0);
  context.setMainFunction(mainFunction);
}

