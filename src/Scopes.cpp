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

Value* Scopes::getVariable(string name) {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  std::vector<Scope *>::iterator iterator = mScopes.end();
  do {
    --iterator;
    Value* value = (*iterator)->getLocals()[name].getValue();
    if (value != NULL) {
      return value;
    }
  } while (iterator != mScopes.begin());
  
  return NULL;
}

void Scopes::setStackVariable(string name, Value* value) {
  Variable variable(value, STACK_VARIABLE);
  getScope()->getLocals()[name] = variable;
}

void Scopes::setHeapVariable(string name, Value* value) {
  Variable variable(value, HEAP_VARIABLE);
  getScope()->getLocals()[name] = variable;
}

void Scopes::pushScope() {
  mScopes.push_back(new Scope());
}

void Scopes::popScope(BasicBlock* basicBlock) {
  Scope* top = mScopes.back();
  top->maybeFreeOwnedMemory(basicBlock);
  mScopes.pop_back();
  delete top;
}

Scope* Scopes::getScope() {
  if (mScopes.size() == 0) {
    Log::e("Can not get scope. Scope list is empty.");
    exit(1);
  }
  return mScopes.back();
}

void Scopes::setBreakToBlock(BasicBlock* block) {
  mScopes.back()->setBreakToBlock(block);
}

BasicBlock* Scopes::getBreakToBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  std::vector<Scope *>::iterator iterator = mScopes.end();
  do {
    --iterator;
    BasicBlock* block = (*iterator)->getBreakToBlock();
    if (block != NULL) {
      return block;
    }
  } while (iterator != mScopes.begin());
  
  return NULL;
}

void Scopes::setContinueToBlock(BasicBlock* block) {
  mScopes.back()->setContinueToBlock(block);
}

BasicBlock* Scopes::getContinueToBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  std::vector<Scope *>::iterator iterator = mScopes.end();
  do {
    --iterator;
    BasicBlock* block = (*iterator)->getContinueToBlock();
    if (block != NULL) {
      return block;
    }
  } while (iterator != mScopes.begin());
  
  return NULL;
}

