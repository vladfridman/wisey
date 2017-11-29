//
//  NullPointerExceptionFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullPointerExceptionFunction.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* NullPointerExceptionFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());

  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);

  return function;
}

string NullPointerExceptionFunction::getName() {
  return "__checkForNullAndThrow";
}

Function* NullPointerExceptionFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void NullPointerExceptionFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("pointer");
  
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, llvmArgument, null, "cmp");
  FakeExpression* fakeExpression = new FakeExpression(compare, PrimitiveTypes::BOOLEAN_TYPE);
  
  Block* thenBlock = new Block();
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(Names::getLangPackageName(),
                                                                  Names::getNPEModelName());
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier, objectBuilderArgumnetList);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder, 0);
  thenBlock->getStatements().push_back(throwStatement);
  CompoundStatement* thenStatement = new CompoundStatement(thenBlock, 0);
  IfStatement ifStatement(fakeExpression, thenStatement);
  
  context.getScopes().pushScope();
  ifStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  IRWriter::createReturnInst(context, NULL);
}
