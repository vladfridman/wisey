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
  
  return generateMain(context, programInterfaceSpecifier);
}

Value* ProgramSuffix::generateMain(IRGenerationContext& context,
                                   InterfaceTypeSpecifier* programInterfaceSpecifier) const {
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
  Injector* injector = new Injector(programInterfaceSpecifier, injectionArguments, 0);
  Identifier* programIdentifier = new Identifier("program");
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  programInterfaceSpecifier = new InterfaceTypeSpecifier(packageExpression,
                                                         Names::getIProgramName());
  ObjectOwnerTypeSpecifier* programObjectOwnerTypeSpecifier =
  new ObjectOwnerTypeSpecifier(programInterfaceSpecifier);
  VariableDeclaration* programVariableDeclaration =
  VariableDeclaration::createWithAssignment(programObjectOwnerTypeSpecifier,
                                            programIdentifier,
                                            injector,
                                            0);
  programVariableDeclaration->generateIR(context);
  delete programVariableDeclaration;

  Thread* mainThread = context.getThread(Names::getMainThreadFullName());
  injectionArguments.push_back(new InjectionArgument("withProgram", new Identifier("program")));
  Value* injectedThread = mainThread->inject(context, injectionArguments, 0);
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName());
  packageType = context.getPackageType(Names::getLangPackageName());
  packageExpression = new FakeExpression(NULL, packageType);
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
  ReturnStatement returnStatement(startMethodCall, 0);
  returnStatement.generateIR(context);

  context.getScopes().popScope(context, 0);
  context.setMainFunction(mainFunction);
  
  return mainFunction;
}

