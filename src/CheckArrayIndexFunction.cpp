//
//  CheckArrayIndexFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/CheckArrayIndexFunction.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/HeapBuilder.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* CheckArrayIndexFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void CheckArrayIndexFunction::call(IRGenerationContext& context,
                                   Value* index,
                                   Value* size,
                                   int line) {
  Function* function = get(context);
  vector<Value*> arguments;
  IVariable* callstackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  arguments.push_back(callstackVariable->generateIdentifierIR(context, line));
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), line));
  arguments.push_back(index);
  arguments.push_back(size);
  
  IRWriter::createInvokeInst(context, function, arguments, "", 0);
}

string CheckArrayIndexFunction::getName() {
  return "__checkArrayIndexFunction";
}

Function* CheckArrayIndexFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* CheckArrayIndexFunction::getLLVMFunctionType(IRGenerationContext& context) {
  const Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);
  
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(callstack);
  argumentTypes.push_back(PrimitiveTypes::INT);
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void CheckArrayIndexFunction::compose(IRGenerationContext& context, Function* function) {
  Function::arg_iterator llvmArguments = function->arg_begin();
  Value* callstackArgument = &*llvmArguments;
  callstackArgument->setName(ThreadExpression::CALL_STACK);
  llvmArguments++;
  Value* lineNumber = &*llvmArguments;
  lineNumber->setName("lineNumber");
  llvmArguments++;
  Value* index = &*llvmArguments;
  index->setName("index");
  llvmArguments++;
  Value* size = &*llvmArguments;
  size->setName("size");
  
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* throwBlock = BasicBlock::Create(llvmContext, "throw", function);
  BasicBlock* returnBlock = BasicBlock::Create(llvmContext, "return", function);
  context.setBasicBlock(entryBlock);
  
  llvm::Constant* zero = ConstantInt::get(PrimitiveTypes::LONG->getLLVMType(context), 0);
  Value* compareToSize = IRWriter::newICmpInst(context, ICmpInst::ICMP_SGE, index, size, "cmp");
  Value* compareToZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, index, zero, "cmp");
  Value* condition = IRWriter::createBinaryOperator(context,
                                                    Instruction::Or,
                                                    compareToSize,
                                                    compareToZero,
                                                    "");
  IRWriter::createConditionalBranch(context, throwBlock, returnBlock, condition);
  
  context.setBasicBlock(returnBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(throwBlock);
  Composer::setLineNumberAtRuntime(context, callstackArgument, lineNumber);
  context.getScopes().pushScope();
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(packageExpression, Names::getArrayIndexOutOfBoundsModelName(), 0);
  BuilderArgumentList builderArgumnetList;
  FakeExpression* sizeExpression = new FakeExpression(size, PrimitiveTypes::LONG);
  BuilderArgument* argument = new BuilderArgument("withArraySize", sizeExpression);
  builderArgumnetList.push_back(argument);
  FakeExpression* indexExpression = new FakeExpression(index, PrimitiveTypes::LONG);
  argument = new BuilderArgument("withIndex", indexExpression);
  builderArgumnetList.push_back(argument);
  HeapBuilder* heapBuilder = new HeapBuilder(modelTypeSpecifier, builderArgumnetList, 0);
  ThrowStatement* throwStatement = new ThrowStatement(heapBuilder, 0);
  throwStatement->generateIR(context);
  context.getScopes().popScope(context, 0);
  IRWriter::createReturnInst(context, NULL);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}

