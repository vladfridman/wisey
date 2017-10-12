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
#include "wisey/EmptyStatement.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceInjector.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/ThrowStatement.hpp"
#include "wisey/TryCatchStatement.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* ProgramSuffix::generateIR(IRGenerationContext& context) const {
  composeNPEFunctionBody(context);
  
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  InterfaceTypeSpecifier* programInterfaceSpecifier =
    new InterfaceTypeSpecifier(package, Names::getIProgramName());
  Interface* interface = (Interface*) programInterfaceSpecifier->getType(context);
  if (!context.hasBoundController(interface)) {
    return NULL;
  }
  
  return generateMain(context, programInterfaceSpecifier);
}

void ProgramSuffix::composeNPEFunctionBody(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getModule()->getFunction(Names::getNPECheckFunctionName());
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("pointer");
  
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, llvmArgument, null, "cmp");
  FakeExpression* fakeExpression = new FakeExpression(compare, PrimitiveTypes::BOOLEAN_TYPE);
  
  Block* thenBlock = new Block();
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(package,
                                                                  Names::getNPEModelName());
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier, objectBuilderArgumnetList);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder);
  thenBlock->getStatements().push_back(throwStatement);
  CompoundStatement* thenStatement = new CompoundStatement(thenBlock);
  IfStatement ifStatement(fakeExpression, thenStatement);
  
  context.getScopes().pushScope();
  ifStatement.generateIR(context);
  context.getScopes().getScope()->removeException(context.getModel(Names::getNPEModelName()));
  context.getScopes().popScope(context);
  
  IRWriter::createReturnInst(context, NULL);
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
  
  InterfaceInjector* interfaceInjector = new InterfaceInjector(programInterfaceSpecifier);
  Identifier* programIdentifier = new Identifier("program", "program");
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  programInterfaceSpecifier = new InterfaceTypeSpecifier(package, Names::getIProgramName());
  VariableDeclaration programVariableDeclaration(programInterfaceSpecifier,
                                                 programIdentifier,
                                                 interfaceInjector);
  programVariableDeclaration.generateIR(context);

  IntConstant* exceptionIntConstant = new IntConstant(11);
  ReturnStatement* exceptionReturnStatement = new ReturnStatement(exceptionIntConstant);
  vector<string> packageSpecifier;
  ModelTypeSpecifier* npeTypeSpecifier = new ModelTypeSpecifier(packageSpecifier,
                                                                Names::getNPEModelName());
  Catch* catchClause = new Catch(npeTypeSpecifier, "exception", exceptionReturnStatement);
  vector<Catch*> catchList;
  catchList.push_back(catchClause);

  Controller* threadController = context.getController("wisey.lang.CThread");
  ExpressionList injectionArguments;
  Value* injectedThread = threadController->inject(context, injectionArguments);
  Value* threadStore = IRWriter::newAllocaInst(context, injectedThread->getType(), "threadStore");
  Value* threadTemp = IRWriter::newAllocaInst(context, injectedThread->getType(), "threadTemp");
  IRWriter::newStoreInst(context, injectedThread, threadTemp);
  IVariable* threadVariable = new HeapOwnerVariable("thread",
                                                    threadController->getOwner(),
                                                    threadStore);
  context.getScopes().setVariable(threadVariable);
  threadVariable->setToNull(context);
  
  FakeExpression* threadExpression = new FakeExpression(threadTemp,
                                                        threadController->getOwner());
  threadVariable->generateAssignmentIR(context, threadExpression);

  ExpressionList runMethodArguments;
  programIdentifier = new Identifier("program", "program");
  MethodCall* runMethodCall = new MethodCall(programIdentifier, "run", runMethodArguments);
  
  ReturnStatement* returnStatement = new ReturnStatement(runMethodCall);
 
  Block* tryBlock = new Block();
  tryBlock->getStatements().push_back(returnStatement);
  CompoundStatement* tryCompoundStatement = new CompoundStatement(tryBlock);
  EmptyStatement* emptyStatement = new EmptyStatement();
  TryCatchStatement tryCatchStatement(tryCompoundStatement, catchList, emptyStatement);
  tryCatchStatement.generateIR(context);
  
  IntConstant* normalIntConstant = new IntConstant(-5);
  ReturnStatement normalReturnStatement(normalIntConstant);
  normalReturnStatement.generateIR(context);

  context.getScopes().popScope(context);
  context.setMainFunction(mainFunction);
  
  return mainFunction;
}

