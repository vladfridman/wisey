//
//  Scope.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scope.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Scope::~Scope() {
  for (map<string, IVariable*>::const_iterator iterator = mVariables.begin();
       iterator != mVariables.end();
       iterator++) {
    delete(iterator->second);
  }
  mVariables.clear();
  if (mExceptionFinally) {
    delete mExceptionFinally;
  }
  mExceptions.clear();
}

IVariable* Scope::findVariable(string name) {
  if (!mVariables.count(name)) {
    return NULL;
  }

  return mVariables.at(name);
}

void Scope::setVariable(string name, IVariable* variable) {
  mVariables[name] = variable;
}

void Scope::eraseClearedVariables(map<string, IVariable*>& clearedVariables) {
  vector<string> variablesToErase;
  for (map<string, IVariable*>::iterator iterator = mVariables.begin();
       iterator != mVariables.end();
       iterator++) {
    string name = iterator->first;
    if (clearedVariables.count(name)) {
      variablesToErase.push_back(name);
    }
  }
  for (string name : variablesToErase) {
    clearedVariables.erase(name);
    mVariables.erase(name);
  }
}

void Scope::setBreakToBlock(BasicBlock* block) {
  mBreakToBlock = block;
}

BasicBlock* Scope::getBreakToBlock() {
  return mBreakToBlock;
}

void Scope::setContinueToBlock(BasicBlock* block) {
  mContinueToBlock = block;
}

BasicBlock* Scope::getContinueToBlock() {
  return mContinueToBlock;
}

void Scope::setLandingPadBlock(BasicBlock* block) {
  mLandingPadBlock = block;
}

BasicBlock* Scope::getLandingPadBlock() {
  return mLandingPadBlock;
}

void Scope::setExceptionContinueBlock(BasicBlock* block) {
  mExceptionContinueBlock = block;
}

BasicBlock* Scope::getExceptionContinueBlock() {
  return mExceptionContinueBlock;
}

void Scope::setExceptionFinally(const IStatement* finallyStatement) {
  mExceptionFinally = finallyStatement;
}

const IStatement* Scope::getExceptionFinally() {
  return mExceptionFinally;
}

void Scope::setReturnType(const IType* type) {
  mReturnType = type;
}

const IType* Scope::getReturnType() {
  return mReturnType;
}

void Scope::freeOwnedMemory(IRGenerationContext& context) {
  if (mHasOwnedMemoryBeenFreed) {
    return;
  }
  
  BasicBlock* currentBlock = context.getBasicBlock();
  if(currentBlock != NULL &&
     currentBlock->size() > 0 &&
     UnreachableInst::classof(&currentBlock->back())) {
    return;
  }
  
  for (map<string, IVariable*>::iterator iterator = mVariables.begin();
       iterator != mVariables.end();
       iterator++) {
    iterator->second->free(context);
  }
  
  mHasOwnedMemoryBeenFreed = true;
}

void Scope::addException(const IType* exception) {
  if (mExceptions.count(exception->getName())) {
    return;
  }
  mExceptions[exception->getName()] = exception;
}

void Scope::addExceptions(vector<const IType*> exceptions) {
  for (const IType* exception : exceptions) {
    addException(exception);
  }
}

void Scope::removeException(const IType* exception) {
  mExceptions.erase(exception->getName());
}

map<string, const IType*> Scope::getExceptions() {
  return mExceptions;
}
