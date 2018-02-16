//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/Injector.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/ObjectOwnerTypeSpecifier.hpp"
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
  
  Controller* threadController = context.getController("wisey.lang.CThread");
  InjectionArgumentList injectionArguments;
  Value* injectedThread = threadController->inject(context, injectionArguments, 0);
  Value* threadStore = IRWriter::newAllocaInst(context, injectedThread->getType(), "threadStore");
  IOwnerVariable* threadVariable = new LocalOwnerVariable(ThreadExpression::THREAD,
                                                          threadController->getOwner(),
                                                          threadStore);
  context.getScopes().setVariable(threadVariable);
  threadVariable->setToNull(context);
  FakeExpression* threadExpression = new FakeExpression(injectedThread,
                                                        threadController->getOwner());
  vector<const IExpression*> arrayIndices;
  threadVariable->generateAssignmentIR(context, threadExpression, arrayIndices, 0);

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

  ExpressionList argumentList;
  programIdentifier = new Identifier("program");
  argumentList.push_back(programIdentifier);
  packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifier* programRunnerControllerSpecifier =
    new ControllerTypeSpecifier(packageExpression, "CProgramRunner");
  StaticMethodCall* runnerCall = new StaticMethodCall(programRunnerControllerSpecifier,
                                                      "run",
                                                      argumentList,
                                                      0);
  ReturnStatement* returnStatement = new ReturnStatement(runnerCall, 0);
  returnStatement->generateIR(context);

  context.getScopes().popScope(context, 0);
  context.setMainFunction(mainFunction);
  
  return mainFunction;
}

