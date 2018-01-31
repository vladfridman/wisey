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
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void CheckArrayIndexFunction::compose(IRGenerationContext& context, Function* function) {
  Function::arg_iterator llvmArguments = function->arg_begin();
  Argument* index = &*llvmArguments;
  index->setName("index");
  llvmArguments++;
  Argument* size = &*llvmArguments;
  size->setName("size");

  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  llvm::Constant* zero = ConstantInt::get(PrimitiveTypes::LONG_TYPE->getLLVMType(context), 0);
  Value* compareToSize = IRWriter::newICmpInst(context, ICmpInst::ICMP_SGE, index, size, "cmp");
  Value* compareToZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, index, zero, "cmp");
  Value* compare = IRWriter::createBinaryOperator(context,
                                                  Instruction::Or,
                                                  compareToSize,
                                                  compareToZero,
                                                  "");
  FakeExpression* compareExpression = new FakeExpression(compare, PrimitiveTypes::BOOLEAN_TYPE);
  
  Block* thenBlock = new Block();
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(packageExpression, Names::getArrayIndexOutOfBoundsModelName());
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  FakeExpression* sizeExpression = new FakeExpression(size, PrimitiveTypes::LONG_TYPE);
  ObjectBuilderArgument* argument = new ObjectBuilderArgument("withArraySize", sizeExpression);
  objectBuilderArgumnetList.push_back(argument);
  FakeExpression* indexExpression = new FakeExpression(index, PrimitiveTypes::LONG_TYPE);
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
}

