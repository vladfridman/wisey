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

void Scopes::setVariable(IVariable* variable) {
  if (getVariable(variable->getName())) {
    Log::e("Already declared variable named '" + variable->getName() +
           "'. Variable hiding is not allowed.");
    exit(1);
  }
  getScope()->setVariable(variable->getName(), variable);
  if (variable->getType()->getTypeKind() != PRIMITIVE_TYPE) {
    clearCachedLandingPadBlock();
  }
}

void Scopes::pushScope() {
  mScopes.push_front(new Scope());
}

void Scopes::popScope(IRGenerationContext& context) {
  Scope* top = mScopes.front();

  top->freeOwnedMemory(context);
  clearCachedLandingPadBlock();

  map<string, const Model*> exceptions = top->getExceptions();
  mScopes.pop_front();
  delete top;
  
  if (exceptions.size() == 0) {
    return;
  }
  
  if (mScopes.size() == 0) {
    reportUnhandledExceptions(exceptions);
    return;
  }
  
  top = mScopes.front();
  for (map<string, const Model*>::iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    top->addException(iterator->second);
  }
}

Scope* Scopes::getScope() {
  if (mScopes.size() == 0) {
    Log::e("Can not get scope. Scope list is empty.");
    exit(1);
  }
  return mScopes.front();
}

list<Scope*> Scopes::getScopesList() {
  return mScopes;
}

void Scopes::freeOwnedMemory(IRGenerationContext& context) {
  for (Scope* scope : mScopes) {
    scope->freeOwnedMemory(context);
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

void Scopes::setObjectType(const IObjectType* objectType) {
  getScope()->setObjectType(objectType);
}

const IObjectType* Scopes::getObjectType() const {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    const IObjectType* objectType = scope->getObjectType();
    if (objectType != NULL) {
      return objectType;
    }
  }
  
  return NULL;
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

void Scopes::reportUnhandledExceptions(map<string, const Model*> exceptions) {
  for (map<string, const Model*>::iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    if (!iterator->first.find(Names::getLangPackageName())) {
      continue;
    }
    Log::e("Exception " + iterator->first + " is not handled");
    exit(1);
  }
}

BasicBlock* Scopes::getLandingPadBlock(IRGenerationContext& context) {
  if (mCachedLandingPadBlock) {
    return mCachedLandingPadBlock;
  }
  
  TryCatchInfo* tryCatchInfo = getTryCatchInfo();
  if (tryCatchInfo) {
    BasicBlock* freeMemoryBlock = freeMemoryAllocatedInTry(context);
    mCachedLandingPadBlock = tryCatchInfo->defineLandingPadBlock(context, freeMemoryBlock);
  } else {
    mCachedLandingPadBlock = Cleanup::generate(context);
  }
  
  return mCachedLandingPadBlock;
}

void Scopes::clearCachedLandingPadBlock() {
  mCachedLandingPadBlock = NULL;
}

llvm::BasicBlock* Scopes::freeMemoryAllocatedInTry(IRGenerationContext& context) {
  Function* function = context.getBasicBlock()->getParent();
  BasicBlock* freeMemoryBlock = BasicBlock::Create(context.getLLVMContext(), "freeMem", function);
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  context.setBasicBlock(freeMemoryBlock);
  
  for (Scope* scope : mScopes) {
    scope->freeOwnedMemory(context);
    if (scope->getTryCatchInfo()) {
      break;
    }
  }

  context.setBasicBlock(lastBasicBlock);
  
  return freeMemoryBlock;
}
