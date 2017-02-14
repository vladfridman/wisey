//
//  Scopes.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/Scopes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

IVariable* Scopes::getVariable(string name) {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for(list<Scope *>::iterator iterator = mScopes.begin(); iterator != mScopes.end(); iterator++) {
    IVariable* variable = (*iterator)->findVariable(name);
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
  
  for(list<Scope *>::iterator iterator = mScopes.begin(); iterator != mScopes.end(); iterator++) {
    IVariable* variable = (*iterator)->findVariable(name);
    if (variable != NULL) {
      (*iterator)->clearVariable(name);
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

void Scopes::pushScope() {
  mScopes.push_front(new Scope());
}

void Scopes::popScope(BasicBlock* basicBlock) {
  Scope* top = mScopes.front();
  top->maybeFreeOwnedMemory(basicBlock);
  mScopes.pop_front();
  delete top;
}

Scope* Scopes::getScope() {
  if (mScopes.size() == 0) {
    Log::e("Can not get scope. Scope list is empty.");
    exit(1);
  }
  return mScopes.front();
}

void Scopes::setBreakToBlock(BasicBlock* block) {
  getScope()->setBreakToBlock(block);
}

BasicBlock* Scopes::getBreakToBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for(list<Scope *>::iterator iterator = mScopes.begin(); iterator != mScopes.end(); iterator++) {
    BasicBlock* block= (*iterator)->getBreakToBlock();
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
 
  for(list<Scope *>::iterator iterator = mScopes.begin(); iterator != mScopes.end(); iterator++) {
    BasicBlock* block = (*iterator)->getContinueToBlock();
    if (block != NULL) {
      return block;
    }
  }

  return NULL;
}

void Scopes::setReturnType(IType* type) {
  getScope()->setReturnType(type);
}

IType* Scopes::getReturnType() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for(list<Scope *>::iterator iterator = mScopes.begin(); iterator != mScopes.end(); iterator++) {
    IType* returnType = (*iterator)->getReturnType();
    if (returnType != NULL) {
      return returnType;
    }
  }
  
  return NULL;
}
