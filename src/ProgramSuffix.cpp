//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Block.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceInjector.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/TryCatchStatement.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* ProgramSuffix::generateIR(IRGenerationContext& context) const {
  vector<string> package;
  InterfaceTypeSpecifier* programInterfaceSpecifier =
    new InterfaceTypeSpecifier(package, Names::getIProgramName());
  Interface* interface = (Interface*) programInterfaceSpecifier->getType(context);
  if (!context.hasBoundController(interface)) {
    return NULL;
  }
  
  return generateMain(context, programInterfaceSpecifier);
}

Value* ProgramSuffix::generateMain(IRGenerationContext& context,
                                   InterfaceTypeSpecifier* programInterfaceSpecifier) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<string> package;

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

