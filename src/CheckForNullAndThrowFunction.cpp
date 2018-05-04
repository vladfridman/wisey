//
//  CheckForNullAndThrowFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
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

Function* CheckForNullAndThrowFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());

  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);

  return function;
}

void CheckForNullAndThrowFunction::call(IRGenerationContext& context, Value* value) {
  llvm::PointerType* int8PointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8PointerType);
  
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  IRWriter::createInvokeInst(context, function, arguments, "", 0);
}

string CheckForNullAndThrowFunction::getName() {
  return "__checkForNullAndThrow";
}

Function* CheckForNullAndThrowFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* CheckForNullAndThrowFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void CheckForNullAndThrowFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("pointer");
  
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, llvmArgument, null, "cmp");
  FakeExpression* fakeExpression = new FakeExpression(compare, PrimitiveTypes::BOOLEAN);
  
  Block* thenBlock = new Block();
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(packageExpression,
                                                                  Names::getNPEModelName(),
                                                                  0);
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder, 0);
  thenBlock->getStatements().push_back(throwStatement);
  CompoundStatement* thenStatement = new CompoundStatement(thenBlock, 0);
  IfStatement ifStatement(fakeExpression, thenStatement);
  
  context.getScopes().pushScope();
  ifStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  IRWriter::createReturnInst(context, NULL, 0);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
