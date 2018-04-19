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
#include "wisey/CastObjectFunction.hpp"
#include "wisey/CheckArrayIndexFunction.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/DestroyNativeObjectFunction.hpp"
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
#include "wisey/InstanceOfFunction.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintErrStatement.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/RelationalExpression.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/StringLiteral.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"
#include "wisey/VariableDeclaration.hpp"

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
  CastObjectFunction::get(context);
  CheckArrayIndexFunction::get(context);
  DestroyNativeObjectFunction::get(context);
  DestroyOwnerArrayFunction::get(context);
  DestroyOwnerObjectFunction::get(context);
  DestroyPrimitiveArrayFunction::get(context);
  DestroyReferenceArrayFunction::get(context);
  GetOriginalObjectFunction::get(context);
  InstanceOfFunction::get(context);
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
  Value* threadNullValue = ConstantPointerNull::get(threadInterface->getLLVMType(context));
  IReferenceVariable* threadVariable =
  new ParameterSystemReferenceVariable(ThreadExpression::THREAD, threadInterface, threadNullValue);
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
  
  IdentifierChain* startMethod = new IdentifierChain(new Identifier("mainThread", 0), "start", 0);
  ExpressionList callArguments;
  MethodCall* startMethodCall = new MethodCall(startMethod, callArguments, 0);
  ExpressionStatement startMethodCallStatement(startMethodCall);
  startMethodCallStatement.generateIR(context);
  
  IdentifierChain* waitMethod = new IdentifierChain(new Identifier("mainThread", 0),
                                                    "awaitResult",
                                                    0);
  MethodCall* waitMethodCall = new MethodCall(waitMethod, callArguments, 0);
  
  PackageType* langPackageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* langPackageExpression = new FakeExpression(NULL, langPackageType);
  ModelTypeSpecifier* mainThreadMessageSpecifier =
  new ModelTypeSpecifier(langPackageExpression, Names::getProgramResultShortName(), 0);
  CastExpression* castToMessage = new CastExpression(mainThreadMessageSpecifier, waitMethodCall, 0);
  
  langPackageType = context.getPackageType(Names::getLangPackageName());
  langPackageExpression = new FakeExpression(NULL, langPackageType);
  mainThreadMessageSpecifier =
  new ModelTypeSpecifier(langPackageExpression, Names::getProgramResultShortName(), 0);
  VariableDeclaration* resultDeclaration =
  VariableDeclaration::createWithAssignment(mainThreadMessageSpecifier,
                                            new Identifier("resultObject", 0),
                                            castToMessage,
                                            0);
  resultDeclaration->generateIR(context);
  
  NullExpression* nullExpression = new NullExpression(0);
  RelationalExpression* condition = new RelationalExpression(new Identifier("resultObject", 0),
                                                             RELATIONAL_OPERATION_EQ,
                                                             nullExpression,
                                                             0);
  ExpressionList expressionsToPrint;
  expressionsToPrint.push_back(new StringLiteral("Main thread ended without a result\n", 0));
  PrintErrStatement* printerr = new PrintErrStatement(expressionsToPrint);
  Block* block = new Block();
  block->getStatements().push_back(printerr);
  IntConstant* intConstant = new IntConstant(1, 0);
  ReturnStatement* returnErrorStatement = new ReturnStatement(intConstant, 0);
  block->getStatements().push_back(returnErrorStatement);
  CompoundStatement* thenStatement = new CompoundStatement(block, 0);
  
  IfStatement ifStatement(condition, thenStatement);
  ifStatement.generateIR(context);
  
  IdentifierChain* getContentMethod = new IdentifierChain(new Identifier("resultObject", 0),
                                                          "getResult",
                                                          0);
  MethodCall* getContent = new MethodCall(getContentMethod, callArguments, 0);
  ReturnStatement returnResultStatement(getContent, 0);
  returnResultStatement.generateIR(context);

  context.getScopes().popScope(context, 0);
  context.setMainFunction(mainFunction);

  delete resultDeclaration;
}

