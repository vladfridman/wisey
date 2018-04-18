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
#include "wisey/LocalSystemReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LLVMFunction::LLVMFunction(const IObjectType* objectType,
                           string name,
                           AccessLevel accessLevel,
                           const LLVMFunctionType* llvmFunctionType,
                           const IType* returnType,
                           vector<const LLVMFunctionArgument*> arguments,
                           CompoundStatement* compoundStatement,
                           int line) :
mObjectType(objectType),
mName(name),
mIsPublic(accessLevel == PUBLIC_ACCESS),
mLLVMFunctionType(llvmFunctionType),
mReturnType(returnType),
mArguments(arguments),
mCompoundStatement(compoundStatement),
mLine(line) {
  assert(returnType->isNative());
}

LLVMFunction::~LLVMFunction() {
}

Value* LLVMFunction::declareFunction(IRGenerationContext& context,
                                     const IObjectType* objectType) const {
  vector<const IType*> argumentTypes;
  for (const LLVMFunctionArgument* argument : mArguments) {
    argumentTypes.push_back(argument->getType());
  }
  LLVMFunctionType* functionType = context.getLLVMFunctionType(mReturnType, argumentTypes);
  string name = IMethodCall::translateObjectMethodToLLVMFunctionName(objectType, mName);
  context.registerLLVMExternalFunctionNamedType(name, functionType);

  return Function::Create(functionType->getLLVMType(context),
                          GlobalValue::InternalLinkage,
                          name,
                          context.getModule());
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
  createSystemVariables(context, function);
  mCompoundStatement->generateIR(context);
  
  maybeAddImpliedVoidReturn(context, mLine);
  
  scopes.popScope(context, mLine);
}

Function* LLVMFunction::getLLVMType(IRGenerationContext& context) const {
  string name = IMethodCall::translateObjectMethodToLLVMFunctionName(mObjectType, mName);
  Function* function = context.getModule()->getFunction(name);
  assert(function != NULL);

  return function;
}

bool LLVMFunction::isPublic() const {
  return mIsPublic;
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
    Log::e_deprecated("LLVM function " + mName +
           " must return a value of type " + mReturnType->getTypeName());
    exit(1);
  }
  
  context.getScopes().freeOwnedMemory(context, line);
  IRWriter::createReturnInst(context, NULL);
}

void LLVMFunction::createSystemVariables(IRGenerationContext& context, Function* function) const {
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), mLine);
  Value* thredStore = IRWriter::newAllocaInst(context,
                                              threadInterface->getLLVMType(context),
                                              "thread");
  LocalSystemReferenceVariable* threadVariable =
  new LocalSystemReferenceVariable(ThreadExpression::THREAD, threadInterface, thredStore);
  context.getScopes().setVariable(threadVariable);
  
  Controller* callStackController = context.getController(Names::getCallStackControllerFullName(),
                                                          mLine);
  llvm::PointerType* callStackLLVMType =
  (llvm::PointerType*) callStackController->getLLVMType(context);
  Value* callStackStore = IRWriter::newAllocaInst(context, callStackLLVMType, "callstack");
  LocalSystemReferenceVariable* callStackVariable =
  new LocalSystemReferenceVariable(ThreadExpression::CALL_STACK,
                                   callStackController,
                                   callStackStore);
  context.getScopes().setVariable(callStackVariable);
}

string LLVMFunction::getName() const {
  return mName;
}

const LLVMFunctionType* LLVMFunction::getType() const {
  return mLLVMFunctionType;
}
