//
//  Scopes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <set>

#include "wisey/Cleanup.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/Scopes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Scopes::Scopes() : mCachedLandingPadBlock(NULL) {
}

Scopes::~Scopes() {
}

IVariable* Scopes::getVariable(string name) {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    IVariable* variable = scope->findVariable(name);
    if (variable != NULL) {
      return variable;
    }
  }
  
  return NULL;
}

void Scopes::setVariable(IRGenerationContext& context, IVariable* variable) {
  IVariable* existingVariable = getVariable(variable->getName());
  if (existingVariable) {
    context.reportError(variable->getLine(),
                        "Variable '" + variable->getName() + "' is already defined on line " +
                        to_string(existingVariable->getLine()) +
                        ", variable hiding is not allowed");
    throw 1;
  }
  getScope()->setVariable(variable->getName(), variable);
  if (!variable->getType()->isPrimitive()) {
    clearCachedLandingPadBlock();
  }
}

void Scopes::pushScope() {
  mScopes.push_front(new Scope());
}

void Scopes::popScope(IRGenerationContext& context, int line) {
  Scope* top = mScopes.front();

  top->freeOwnedMemory(context, NULL, line);
  clearCachedLandingPadBlock();

  map<string, int> exceptions = top->getExceptions();
  mScopes.pop_front();
  delete top;
  
  if (exceptions.size() == 0) {
    return;
  }
  
  if (mScopes.size() == 0) {
    reportUnhandledExceptions(context, exceptions);
    return;
  }
  
  top = mScopes.front();
  for (map<string, int>::iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    top->addException(context.getModel(iterator->first, line), iterator->second);
  }
}

Scope* Scopes::getScope() {
  assert(mScopes.size() && "Scope list is empty");
  return mScopes.front();
}

list<Scope*> Scopes::getScopesList() {
  return mScopes;
}

void Scopes::freeOwnedMemory(IRGenerationContext& context, Value* exception, int line) {
  for (Scope* scope : mScopes) {
    scope->freeOwnedMemory(context, exception, line);
  }
}

void Scopes::setBreakToBlock(BasicBlock* block) {
  getScope()->setBreakToBlock(block);
}

BasicBlock* Scopes::getBreakToBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    BasicBlock* block= scope->getBreakToBlock();
    if (block != NULL) {
      return block;
    }
  }
  
  return NULL;
}

void Scopes::setContinueToBlock(BasicBlock* block) {
  getScope()->setContinueToBlock(block);
}

BasicBlock* Scopes::getContinueToBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    BasicBlock* block = scope->getContinueToBlock();
    if (block != NULL) {
      return block;
    }
  }
  
  return NULL;
}

void Scopes::beginTryCatch(TryCatchInfo* tryCatchInfo) {
  getScope()->setTryCatchInfo(tryCatchInfo);
  clearCachedLandingPadBlock();
}

TryCatchInfo* Scopes::getTryCatchInfo() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    TryCatchInfo* info = scope->getTryCatchInfo();
    if (info != NULL) {
      return info;
    }
  }
  
  return NULL;
}

bool Scopes::endTryCatch(IRGenerationContext& context) {
  TryCatchInfo* tryCatchInfo = getScope()->getTryCatchInfo();
  getScope()->clearTryCatchInfo();
  return tryCatchInfo->runComposingCallbacks(context);
}

void Scopes::setReturnType(const IType* type) {
  getScope()->setReturnType(type);
}

const IType* Scopes::getReturnType() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    const IType* returnType = scope->getReturnType();
    if (returnType != NULL) {
      return returnType;
    }
  }
  
  return NULL;
}

void Scopes::reportUnhandledExceptions(IRGenerationContext& context,
                                       map<string, int> exceptions) const {
  for (auto iterator = exceptions.begin(); iterator != exceptions.end(); iterator++) {
    if (!iterator->first.find(Names::getLangPackageName()) ||
        !iterator->first.find(Names::getThreadsPackageName()) ||
        !iterator->first.find(Names::getIOPackageName())) {
      continue;
    }
    context.reportError(iterator->second, "Exception " + iterator->first + " is not handled");
    throw 1;
  }
}

BasicBlock* Scopes::getLandingPadBlock(IRGenerationContext& context, int line) {
  if (mCachedLandingPadBlock) {
    return mCachedLandingPadBlock;
  }
  
  TryCatchInfo* tryCatchInfo = getTryCatchInfo();
  if (tryCatchInfo) {
    LLVMContext& llvmContext = context.getLLVMContext();
    Function* function = context.getBasicBlock()->getParent();
    BasicBlock* freeMemoryBlock = BasicBlock::Create(llvmContext, "freeMem", function);
    auto landingPadInfo = tryCatchInfo->defineLandingPadBlock(context, freeMemoryBlock);
    mCachedLandingPadBlock = get<0>(landingPadInfo);
    Value* wrappedException = get<1>(landingPadInfo);
    freeMemoryAllocatedInTry(context, freeMemoryBlock, wrappedException, line);
  } else {
    mCachedLandingPadBlock = Cleanup::generate(context, line);
  }
  
  return mCachedLandingPadBlock;
}

void Scopes::clearCachedLandingPadBlock() {
  mCachedLandingPadBlock = NULL;
}

void Scopes::freeMemoryAllocatedInTry(IRGenerationContext& context,
                                      BasicBlock* basicBlock,
                                      Value* wrappedException,
                                      int line) {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  context.setBasicBlock(basicBlock);

  Function* getException = IntrinsicFunctions::getExceptionPointerFunction(context);
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  Value* exceptionShell = IRWriter::createCallInst(context, getException, arguments, "");
  Value* idx[1];
  idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), Environment::getAddressSizeInBytes());
  Value* exception = IRWriter::createGetElementPtrInst(context, exceptionShell, idx);

  for (Scope* scope : mScopes) {
    if (scope->getTryCatchInfo()) {
      break;
    }
    scope->freeOwnedMemory(context, exception, line);
  }

  context.setBasicBlock(lastBasicBlock);
}

void Scopes::clear() {
  mScopes.clear();
}
