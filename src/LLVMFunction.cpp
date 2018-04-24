//
//  LLVMFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/Argument.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalSystemReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodQualifier.hpp"
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
                           vector<const Argument*> arguments,
                           CompoundStatement* compoundStatement,
                           int line) :
mObjectType(objectType),
mName(name),
mIsPublic(accessLevel == PUBLIC_ACCESS),
mLLVMFunctionType(llvmFunctionType),
mReturnType(returnType),
mArguments(arguments),
mCompoundStatement(compoundStatement),
mMethodQualifiers(new MethodQualifiers(line)),
mLine(line) {
  assert(returnType->isNative());
}

LLVMFunction::~LLVMFunction() {
  delete mMethodQualifiers;
}

Value* LLVMFunction::declareFunction(IRGenerationContext& context,
                                     const IObjectType* objectType) const {
  vector<const IType*> argumentTypes;
  for (const Argument* argument : mArguments) {
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

Function* LLVMFunction::getLLVMFunction(IRGenerationContext& context) const {
  string name = IMethodCall::translateObjectMethodToLLVMFunctionName(mObjectType, mName);
  Function* function = context.getModule()->getFunction(name);
  assert(function != NULL);

  return function;
}

const IType* LLVMFunction::getReturnType() const {
  return mLLVMFunctionType->getReturnType();
}

vector<const wisey::Argument*> LLVMFunction::getArguments() const {
  return mArguments;
}

vector<const Model*> LLVMFunction::getThrownExceptions() const {
  vector<const Model*> emptyList;
  return emptyList;
}

bool LLVMFunction::isStatic() const {
  return true;
}

bool LLVMFunction::isOverride() const {
  return false;
}

FunctionType* LLVMFunction::getLLVMType(IRGenerationContext& context) const {
  return mLLVMFunctionType->getLLVMType(context);
}

const IObjectType* LLVMFunction::getParentObject() const {
  return mObjectType;
}

MethodQualifiers* LLVMFunction::getMethodQualifiers() const {
  return mMethodQualifiers;
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
  if (!function->getArgumentList().size()) {
    return;
  }

  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument *llvmFunctionArgument = &*llvmFunctionArguments;
  for (const Argument* argument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(argument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  for (const Argument* methodArgument : mArguments) {
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

string LLVMFunction::getTypeName() const {
  return mObjectType->getTypeName() + "." + getName();
}

bool LLVMFunction::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return false;
}

bool LLVMFunction::canAutoCastTo(IRGenerationContext& context, const IType *toType) const {
  return false;
}

Value* LLVMFunction::castTo(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType,
                            int line) const {
  assert(false);
}

bool LLVMFunction::isPrimitive() const {
  return false;
}

bool LLVMFunction::isOwner() const {
  return false;
}

bool LLVMFunction::isReference() const {
  return false;
}

bool LLVMFunction::isArray() const {
  return false;
}

bool LLVMFunction::isFunction() const {
  return true;
}

bool LLVMFunction::isPackage() const {
  return false;
}

bool LLVMFunction::isController() const {
  return false;
}

bool LLVMFunction::isInterface() const {
  return false;
}

bool LLVMFunction::isModel() const {
  return false;
}

bool LLVMFunction::isNode() const {
  return false;
}

bool LLVMFunction::isNative() const {
  return true;
}

bool LLVMFunction::isPointer() const {
  return false;
}

bool LLVMFunction::isImmutable() const {
  return false;
}

void LLVMFunction::printToStream(IRGenerationContext& context, iostream& stream) const {
  IMethodDescriptor::printDescriptorToStream(this, stream);
}

void LLVMFunction::createLocalVariable(IRGenerationContext& context, string name) const {
  assert(false);
}

void LLVMFunction::createFieldVariable(IRGenerationContext& context,
                                       string name,
                                       const IConcreteObjectType* object) const {
  assert(false);
}

void LLVMFunction::createParameterVariable(IRGenerationContext& context,
                                           string name,
                                           Value* value) const {
  assert(false);
}

const wisey::ArrayType* LLVMFunction::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

Instruction* LLVMFunction::inject(IRGenerationContext& context,
                                  const InjectionArgumentList injectionArgumentList,
                                  int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}
