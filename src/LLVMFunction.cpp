//
//  LLVMFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IMethodCall.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LLVMFunction::LLVMFunction(string name,
                           const LLVMFunctionType* llvmFunctionType,
                           const ILLVMType* returnType,
                           vector<const LLVMFunctionArgument*> arguments,
                           CompoundStatement* compoundStatement,
                           int line) :
mName(name),
mLLVMFunctionType(llvmFunctionType),
mReturnType(returnType),
mArguments(arguments),
mCompoundStatement(compoundStatement),
mLine(line) {
}

LLVMFunction::~LLVMFunction() {
}

Value* LLVMFunction::declareFunction(IRGenerationContext& context,
                                     const IObjectType* objectType) const {
  vector<Type*> argumentTypes;
  for (const LLVMFunctionArgument* argument : mArguments) {
    argumentTypes.push_back(argument->getType()->getLLVMType(context));
  }
  Type* llvmReturnType = mReturnType->getLLVMType(context);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argumentTypes, false);
  string name = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType, mName);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, name, context.getModule());
}

void LLVMFunction::generateBodyIR(IRGenerationContext& context,
                                  const IObjectType* objectType) const {
  string name = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType, mName);
  Function* function = context.getModule()->getFunction(name);
  assert(function != NULL);
  
  Scopes& scopes = context.getScopes();
  
  scopes.pushScope();
  scopes.setReturnType(mReturnType);
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function, 0);
  context.setBasicBlock(basicBlock);
  
  createArguments(context, function);
  mCompoundStatement->generateIR(context);
  
  maybeAddImpliedVoidReturn(context, mLine);
  
  scopes.popScope(context, mLine);
}

Function* LLVMFunction::getLLVMFunction(IRGenerationContext& context,
                                        const IObjectType* objectType) const {
  string name = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType, mName);
  Function* function = context.getModule()->getFunction(name);
  assert(function != NULL);

  return function;
}

bool LLVMFunction::isConstant() const {
  return false;
}

bool LLVMFunction::isField() const {
  return false;
}

bool LLVMFunction::isMethod() const {
  return false;
}

bool LLVMFunction::isStaticMethod() const {
  return false;
}

bool LLVMFunction::isMethodSignature() const {
  return false;
}

bool LLVMFunction::isLLVMFunction() const {
  return true;
}

void LLVMFunction::createArguments(IRGenerationContext& context, Function* function) const {
  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument *llvmFunctionArgument = &*llvmFunctionArguments;
  for (const LLVMFunctionArgument* argument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(argument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  for (const LLVMFunctionArgument* methodArgument : mArguments) {
    methodArgument->getType()->createParameterVariable(context,
                                                       methodArgument->getName(),
                                                       &*llvmFunctionArguments);
    llvmFunctionArguments++;
  }
}

void LLVMFunction::maybeAddImpliedVoidReturn(IRGenerationContext& context, int line) const {
  if (context.getBasicBlock()->getTerminator()) {
    return;
  }
  
  if (mReturnType != LLVMPrimitiveTypes::VOID) {
    Log::e("LLVM function " + mName +
           " must return a value of type " + mReturnType->getTypeName());
    exit(1);
  }
  
  context.getScopes().freeOwnedMemory(context, line);
  IRWriter::createReturnInst(context, NULL);
}

string LLVMFunction::getName() const {
  return mName;
}

const LLVMFunctionType* LLVMFunction::getType() const {
  return mLLVMFunctionType;
}
