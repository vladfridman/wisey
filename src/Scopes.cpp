//
//  Scopes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <set>

#include "wisey/Cleanup.hpp"
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
  if (getVariable(variable->getName())) {
    Log::e_deprecated("Already declared variable named '" + variable->getName() +
           "'. Variable hiding is not allowed.");
    exit(1);
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

  top->freeOwnedMemory(context, line);
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
  if (mScopes.size() == 0) {
    Log::e_deprecated("Can not get scope. Scope list is empty.");
    exit(1);
  }
  return mScopes.front();
}

list<Scope*> Scopes::getScopesList() {
  return mScopes;
}

void Scopes::freeOwnedMemory(IRGenerationContext& context, int line) {
  for (Scope* scope : mScopes) {
    scope->freeOwnedMemory(context, line);
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
    exit(1);
  }
}

BasicBlock* Scopes::getLandingPadBlock(IRGenerationContext& context, int line) {
  if (mCachedLandingPadBlock) {
    return mCachedLandingPadBlock;
  }
  
  TryCatchInfo* tryCatchInfo = getTryCatchInfo();
  if (tryCatchInfo) {
    BasicBlock* freeMemoryBlock = freeMemoryAllocatedInTry(context, line);
    mCachedLandingPadBlock = tryCatchInfo->defineLandingPadBlock(context, freeMemoryBlock);
  } else {
    mCachedLandingPadBlock = Cleanup::generate(context, line);
  }
  
  return mCachedLandingPadBlock;
}

void Scopes::clearCachedLandingPadBlock() {
  mCachedLandingPadBlock = NULL;
}

llvm::BasicBlock* Scopes::freeMemoryAllocatedInTry(IRGenerationContext& context, int line) {
  Function* function = context.getBasicBlock()->getParent();
  BasicBlock* freeMemoryBlock = BasicBlock::Create(context.getLLVMContext(), "freeMem", function);
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  context.setBasicBlock(freeMemoryBlock);
  
  for (Scope* scope : mScopes) {
    if (scope->getTryCatchInfo()) {
      break;
    }
    scope->freeOwnedMemory(context, line);
  }

  context.setBasicBlock(lastBasicBlock);
  
  return freeMemoryBlock;
}
