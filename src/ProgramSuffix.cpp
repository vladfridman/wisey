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
#include "wisey/OwnerTypeSpecifier.hpp"
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
  composeAdjustReferenceCounterForConcreteObjectUnsafelyFunctionBody(context);
  composeAdjustReferenceCounterForInterfaceFunctionBody(context);

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

void ProgramSuffix::
composeAdjustReferenceCounterForConcreteObjectUnsafelyFunctionBody(IRGenerationContext&
                                                                  context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getModule()->
  getFunction(Names::getAdjustReferenceCounterForConcreteObjectUnsafelyFunctionName());
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("counter");
  Value* counter = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("adjustment");
  Value* adjustment = llvmArgument;

  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  
  context.setBasicBlock(entryBlock);
  Value* null = ConstantPointerNull::get(Type::getInt64Ty(llvmContext)->getPointerTo());
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, counter, null, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* count = IRWriter::newLoadInst(context, counter, "count");
  Value* sum = IRWriter::createBinaryOperator(context, Instruction::Add, count, adjustment, "");
  IRWriter::newStoreInst(context, sum, counter);
  IRWriter::createReturnInst(context, NULL);
}

void ProgramSuffix::
composeAdjustReferenceCounterForInterfaceFunctionBody(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getModule()->
  getFunction(Names::getAdjustReferenceCounterForInterfaceFunctionName());
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* llvmArgument = &*llvmArguments;
  llvmArgument->setName("object");
  Value* object = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("adjustment");
  Value* adjustment = llvmArgument;
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  
  context.setBasicBlock(entryBlock);
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, object, null, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* original = Interface::getOriginalObject(context, object);
  Type* int64PointerType = Type::getInt64Ty(llvmContext)->getPointerTo();
  Value* counter = IRWriter::newBitCastInst(context, original, int64PointerType);
  Value* count = IRWriter::newLoadInst(context, counter, "count");
  Value* sum = IRWriter::createBinaryOperator(context, Instruction::Add, count, adjustment, "");
  IRWriter::newStoreInst(context, sum, counter);
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
  
  Controller* threadController = context.getController("wisey.lang.CThread");
  ExpressionList injectionArguments;
  Value* injectedThread = threadController->inject(context, injectionArguments);
  Value* threadStore = IRWriter::newAllocaInst(context, injectedThread->getType(), "threadStore");
  IOwnerVariable* threadVariable = new LocalOwnerVariable(ThreadExpression::THREAD,
                                                          threadController->getOwner(),
                                                          threadStore);
  context.getScopes().setVariable(threadVariable);
  threadVariable->setToNull(context);
  FakeExpression* threadExpression = new FakeExpression(injectedThread,
                                                        threadController->getOwner());
  threadVariable->generateAssignmentIR(context, threadExpression);

  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");

  Injector* injector = new Injector(programInterfaceSpecifier);
  Identifier* programIdentifier = new Identifier("program");
  programInterfaceSpecifier = new InterfaceTypeSpecifier(package, Names::getIProgramName());
  OwnerTypeSpecifier* programOwnerTypeSpecifier = new OwnerTypeSpecifier(programInterfaceSpecifier);
  VariableDeclaration programVariableDeclaration(programOwnerTypeSpecifier,
                                                 programIdentifier,
                                                 injector);
  programVariableDeclaration.generateIR(context);

  ExpressionList argumentList;
  programIdentifier = new Identifier("program");
  argumentList.push_back(programIdentifier);
  ControllerTypeSpecifier* programRunnerControllerSpecifier =
    new ControllerTypeSpecifier(package, "CProgramRunner");
  StaticMethodCall* runnerCall = new StaticMethodCall(programRunnerControllerSpecifier,
                                                      "run",
                                                      argumentList,
                                                      0);
  ReturnStatement* returnStatement = new ReturnStatement(runnerCall);
  returnStatement->generateIR(context);

  context.getScopes().popScope(context);
  context.setMainFunction(mainFunction);
  
  return mainFunction;
}

