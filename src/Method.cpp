//
//  Method.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/CompoundStatement.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/HeapOwnerMethodParameter.hpp"
#include "wisey/HeapReferenceMethodParameter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Model.hpp"
#include "wisey/StackVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Method::~Method() {
  for (MethodArgument* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  // mCompoundStatement is deleted with MethodDeclaration
}

string Method::getName() const {
  return mName;
}

AccessLevel Method::getAccessLevel() const {
  return mAccessLevel;
}

const IType* Method::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> Method::getArguments() const {
  return mArguments;
}

unsigned long Method::getIndex() const {
  return mIndex;
}

vector<const Model*> Method::getThrownExceptions() const {
  return mThrownExceptions;
}

Function* Method::defineFunction(IRGenerationContext& context, IObjectType* objectType) {
  FunctionType *ftype = IMethodDescriptor::getLLVMFunctionType((IMethodDescriptor*) this,
                                                               context,
                                                               objectType);
  string functionName = MethodCall::translateObjectMethodToLLVMFunctionName(objectType, mName);
  
  mFunction = Function::Create(ftype,
                               GlobalValue::InternalLinkage,
                               functionName,
                               context.getModule());
  
  return mFunction;
}

void Method::generateIR(IRGenerationContext& context, IObjectType* objectType) const {
  assert(mFunction != NULL);
  
  Scopes& scopes = context.getScopes();
  
  map<string, IVariable*> clearedVariablesBefore = scopes.getClearedVariables();
  scopes.pushScope();
  scopes.setReturnType(mReturnType);
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", mFunction, 0);
  context.setBasicBlock(basicBlock);
  
  maybeGenerateCleanupTryCatchInfo(context);
  
  createArguments(context, mFunction, objectType);
  mCompoundStatement->generateIR(context);
  
  maybeAddImpliedVoidReturn(context);

  checkForUnhandledExceptions(context);
  maybeGenerateCleanupLandingPad(context);

  scopes.popScope(context);
  scopes.setClearedVariables(clearedVariablesBefore);
}

void Method::createArguments(IRGenerationContext& context,
                             Function* function,
                             IObjectType* objectType) const {
  Function::arg_iterator llvmFunctionArguments = function->arg_begin();
  llvm::Argument *llvmFunctionArgument = &*llvmFunctionArguments;
  llvmFunctionArgument->setName("this");
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    llvmFunctionArgument = &*llvmFunctionArguments;
    llvmFunctionArgument->setName(methodArgument->getName());
    llvmFunctionArguments++;
  }
  
  llvmFunctionArguments = function->arg_begin();
  storeArgumentValue(context, "this", objectType, &*llvmFunctionArguments);
  llvmFunctionArguments++;
  for (MethodArgument* methodArgument : mArguments) {
    storeArgumentValue(context,
                       methodArgument->getName(),
                       methodArgument->getType(),
                       &*llvmFunctionArguments);
    llvmFunctionArguments++;
  }
}

void Method::storeArgumentValue(IRGenerationContext& context,
                                std::string variableName,
                                const IType* variableType,
                                llvm::Value* variableValue) const {
  if (variableType->getTypeKind() == PRIMITIVE_TYPE) {
    LLVMContext& llvmContext = context.getLLVMContext();
    Type* llvmType = variableType->getLLVMType(llvmContext);
    string newName = variableName + ".param";
    AllocaInst *alloc = IRWriter::newAllocaInst(context, llvmType, newName);
    IRWriter::newStoreInst(context, variableValue, alloc);
    IVariable* variable = new StackVariable(variableName, variableType, alloc);
    context.getScopes().setVariable(variable);
    return;
  }
  
  if (IType::isOwnerType(variableType)) {
    Type* variableLLVMType = variableType->getLLVMType(context.getLLVMContext());
    Value* alloc = IRWriter::newAllocaInst(context, variableLLVMType, "parameterObjectPointer");
    IRWriter::newStoreInst(context, variableValue, alloc);
    IObjectOwnerType* objectOwnerType = (IObjectOwnerType*) variableType;
    IVariable* variable = new HeapOwnerMethodParameter(variableName, objectOwnerType, alloc);
    context.getScopes().setVariable(variable);
    return;
  }
  
  IVariable* variable = new HeapReferenceMethodParameter(variableName, variableType, variableValue);
  context.getScopes().setVariable(variable);
}

void Method::maybeAddImpliedVoidReturn(IRGenerationContext& context) const {
  if (context.getBasicBlock()->getTerminator()) {
    return;
  }

  context.getScopes().freeOwnedMemory(context);
  IRWriter::createReturnInst(context, NULL);
}

void Method::checkForUnhandledExceptions(IRGenerationContext& context) const {
  Scope* scope = context.getScopes().getScope();
  map<string, const Model*> exceptions = scope->getExceptions();
  if (exceptions.size() == 0) {
    return;
  }
  
  for (const Model* thrownException : mThrownExceptions) {
    scope->removeException(thrownException);
  }
  
  exceptions = scope->getExceptions();
  bool hasUnhandledExceptions = false;
  for (map<string, const Model*>::const_iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    Log::e("Method " + getName() + " neither handles the exception " + iterator->first +
           " nor throws it");
    hasUnhandledExceptions = true;
  }
  
  if (hasUnhandledExceptions) {
    exit(1);
  }
}

void Method::maybeGenerateCleanupTryCatchInfo(IRGenerationContext& context) const {
  if (!mThrownExceptions.size()) {
    return;
  }
  
  BasicBlock* landingPadBlock = BasicBlock::Create(context.getLLVMContext(),
                                                   "cleanup.landing.pad",
                                                   mFunction);
  const IStatement* emptyStatement = new EmptyStatement();
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(landingPadBlock, NULL, emptyStatement, catchList);
  context.getScopes().setTryCatchInfo(tryCatchInfo);
}

void Method::maybeGenerateCleanupLandingPad(IRGenerationContext& context) const {
  if (!mThrownExceptions.size()) {
    return;
  }
  
  context.setBasicBlock(context.getScopes().getTryCatchInfo()->getLandingPadBlock());
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getBasicBlock()->getParent();
  if (!function->hasPersonalityFn()) {
    function->setPersonalityFn(IntrinsicFunctions::getPersonalityFunction(context));
  }
  
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Type*> landingPadReturnTypes;
  landingPadReturnTypes.push_back(int8PointerType);
  landingPadReturnTypes.push_back(Type::getInt32Ty(llvmContext));
  StructType* landingPadReturnType = StructType::get(llvmContext, landingPadReturnTypes);
  LandingPadInst* landingPad = LandingPadInst::Create(landingPadReturnType,
                                                      0,
                                                      "",
                                                      context.getBasicBlock());

  landingPad->setCleanup(true);
  IRWriter::createResumeInst(context, landingPad);
}
