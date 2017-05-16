//
//  ProgramSuffix.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Identifier.hpp"
#include "yazyk/InterfaceInjector.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodCall.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/ProgramSuffix.hpp"
#include "yazyk/ReturnStatement.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ProgramSuffix::generateIR(IRGenerationContext& context) const {
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
  
  InterfaceTypeSpecifier programInterfaceSpecifier("IProgram");
  InterfaceInjector interfaceInjector(programInterfaceSpecifier);
  Identifier programIdentifier("program", "program");
  VariableDeclaration programVariableDeclaration(programInterfaceSpecifier,
                                                 programIdentifier,
                                                 &interfaceInjector);
  programVariableDeclaration.generateIR(context);
  
  ExpressionList runMethodArguments;
  MethodCall runMethodCall(programIdentifier, "run", runMethodArguments);
  ReturnStatement returnStatement(runMethodCall);
  returnStatement.generateIR(context);
  
  context.getScopes().popScope(context);
  context.setMainFunction(mainFunction);
  
  return mainFunction;
}

