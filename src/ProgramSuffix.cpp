//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/CastExpression.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/ExpressionStatement.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/Injector.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/ModelTypeSpecifierFull.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/NullType.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/ObjectOwnerTypeSpecifier.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/StaticMethodCall.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ThrowStatement.hpp"
#include "wisey/TryCatchStatement.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* ProgramSuffix::generateIR(IRGenerationContext& context) const {
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier* programInterfaceSpecifier =
  new InterfaceTypeSpecifier(packageExpression, Names::getIProgramName());
  Interface* interface = (Interface*) programInterfaceSpecifier->getType(context);
  if (!context.hasBoundController(interface)) {
    return NULL;
  }
  
  return generateMain(context);
}

Value* ProgramSuffix::generateMain(IRGenerationContext& context) const {
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
  Thread* mainThread = context.getThread(Names::getMainThreadFullName());
  Value* injectedThread = mainThread->inject(context, injectionArguments, 0);
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName());
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier* threadInterfaceSpecifier =
    new InterfaceTypeSpecifier(packageExpression, Names::getThreadInterfaceName());
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

  Controller* callStack = context.getController(Names::getCallStackControllerFullName());
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
  
  IdentifierChain* waitMethod = new IdentifierChain(new Identifier("mainThread"), "waitForResult");
  MethodCall* waitMethodCall = new MethodCall(waitMethod, callArguments, 0);
  
  packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* mainThreadMessageSpecifier =
  new ModelTypeSpecifier(packageExpression, Names::getMainThreadMessageShortName());
  CastExpression* castToMessage = new CastExpression(mainThreadMessageSpecifier, waitMethodCall, 0);
  IdentifierChain* getContentMethod = new IdentifierChain(castToMessage, "getContent");
  MethodCall* getContent = new MethodCall(getContentMethod, callArguments, 0);
  ReturnStatement returnStatement(getContent, 0);
  returnStatement.generateIR(context);

  context.getScopes().popScope(context, 0);
  context.setMainFunction(mainFunction);
  
  return mainFunction;
}

