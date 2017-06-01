//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Identifier.hpp"
#include "wisey/InterfaceInjector.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

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
  
  vector<string> package;
  InterfaceTypeSpecifier* programInterfaceSpecifier =
    new InterfaceTypeSpecifier(package, "IProgram");
  InterfaceInjector interfaceInjector(programInterfaceSpecifier);
  Identifier* programIdentifier = new Identifier("program", "program");
  programInterfaceSpecifier = new InterfaceTypeSpecifier(package, "IProgram");
  VariableDeclaration programVariableDeclaration(programInterfaceSpecifier,
                                                 *programIdentifier,
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

