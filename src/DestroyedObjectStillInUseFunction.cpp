//
//  DestroyedObjectStillInUseFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/DestroyedObjectStillInUseFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyedObjectStillInUseFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  vector<const IObjectType*> objectTypes;
  context.addComposingCallback(compose, function, objectTypes);
  
  return function;
}

string DestroyedObjectStillInUseFunction::getName() {
  return "__throwDestroyedObjectStillInUse";
}

Function* DestroyedObjectStillInUseFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(functionType,
                          GlobalValue::InternalLinkage,
                          getName(),
                          context.getModule());
}

void DestroyedObjectStillInUseFunction::compose(IRGenerationContext& context,
                                                llvm::Function* function,
                                                vector<const IObjectType*> objectTypes) {
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  ModelTypeSpecifier* modelTypeSpecifier =
  new ModelTypeSpecifier(package, Names::getDestroyedObjectStillInUseName());
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier, objectBuilderArgumnetList);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder);
  
  context.getScopes().pushScope();
  throwStatement->generateIR(context);
  context.getScopes().popScope(context);
  
  IRWriter::createReturnInst(context, NULL);
}
