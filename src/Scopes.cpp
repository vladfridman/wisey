//
//  Scopes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <set>

#include "wisey/EmptyStatement.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IVariable* Scopes::getVariable(string name) {
  if (mClearedVariables.count(name)) {
    return NULL;
  }
  
  return getVariableForAssignement(name);
}

IVariable* Scopes::getVariableForAssignement(string name) {
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

void Scopes::clearVariable(string name) {
  if (mScopes.size() == 0) {
    Log::e("Could not clear variable '" + name + "': the Scopes stack is empty");
    exit(1);
    return;
  }
  
  for (Scope* scope : mScopes) {
    IVariable* variable = scope->findVariable(name);
    if (variable != NULL) {
      mClearedVariables[name] = variable;
      return;
    }
  }
  
  Log::e("Could not clear variable '" + name + "': it was not found");
  exit(1);
  return;
}

void Scopes::setVariable(IVariable* variable) {
  getScope()->setVariable(variable->getName(), variable);
}

map<string, IVariable*> Scopes::getClearedVariables() {
  return mClearedVariables;
}

void Scopes::setClearedVariables(map<string, IVariable*> clearedVariables) {
  mClearedVariables = clearedVariables;
}

void Scopes::eraseFromClearedVariables(IVariable* variable) {
  mClearedVariables.erase(variable->getName());
}

void Scopes::pushScope() {
  mScopes.push_front(new Scope());
}

void Scopes::popScope(IRGenerationContext& context) {
  Scope* top = mScopes.front();

  top->eraseClearedVariables(mClearedVariables);
  top->freeOwnedMemory(context);
  
  map<string, const Model*> exceptions = top->getExceptions();
  mScopes.pop_front();
  delete top;
  
  if (exceptions.size() == 0) {
    return;
  }
  
  if (mScopes.size() == 0) {
    reportUnhandledExceptions(exceptions);
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

void Scopes::freeOwnedMemory(IRGenerationContext& context) {
  for (Scope* scope : mScopes) {
    scope->eraseClearedVariables(mClearedVariables);
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

void Scopes::setTryCatchInfo(TryCatchInfo* tryCatchInfo) {
  getScope()->setTryCatchInfo(tryCatchInfo);
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

void Scopes::clearTryCatchInfo() {
  getScope()->clearTryCatchInfo();
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
    Log::e("Exception " + iterator->first + " is not handled");
  }
  exit(1);
}
