//
//  CheckArrayIndexFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/CheckArrayIndexFunction.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
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

void CheckArrayIndexFunction::call(IRGenerationContext& context, Value* index, Value* size) {
  Function* function = get(context);
  vector<Value*> arguments;
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
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  argumentTypes.push_back(LLVMPrimitiveTypes::I64);
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void CheckArrayIndexFunction::compose(IRGenerationContext& context, Function* function) {
  Function::arg_iterator llvmArguments = function->arg_begin();
  Value* index = &*llvmArguments;
  index->setName("index");
  llvmArguments++;
  Value* size = &*llvmArguments;
  size->setName("size");

  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  llvm::Constant* zero = ConstantInt::get(PrimitiveTypes::LONG->getLLVMType(context), 0);
  Value* compareToSize = IRWriter::newICmpInst(context, ICmpInst::ICMP_SGE, index, size, "cmp");
  Value* compareToZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, index, zero, "cmp");
  Value* compare = IRWriter::createBinaryOperator(context,
                                                  Instruction::Or,
                                                  compareToSize,
                                                  compareToZero,
                                                  "");
  FakeExpression* compareExpression = new FakeExpression(compare, PrimitiveTypes::BOOLEAN);
  
  Block* thenBlock = new Block();
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(packageExpression, Names::getArrayIndexOutOfBoundsModelName(), 0);
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  FakeExpression* sizeExpression = new FakeExpression(size, PrimitiveTypes::LONG);
  ObjectBuilderArgument* argument = new ObjectBuilderArgument("withArraySize", sizeExpression);
  objectBuilderArgumnetList.push_back(argument);
  FakeExpression* indexExpression = new FakeExpression(index, PrimitiveTypes::LONG);
  argument = new ObjectBuilderArgument("withIndex", indexExpression);
  objectBuilderArgumnetList.push_back(argument);
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder, 0);
  thenBlock->getStatements().push_back(throwStatement);
  CompoundStatement* thenStatement = new CompoundStatement(thenBlock, 0);
  IfStatement ifStatement(compareExpression, thenStatement);
  
  context.getScopes().pushScope();
  ifStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  IRWriter::createReturnInst(context, NULL);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context));
}

