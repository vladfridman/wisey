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
#include "wisey/Model.hpp"
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
  if (mTryCatchInfo) {
    delete mTryCatchInfo;
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
  if (IType::isReferenceType(variable->getType())) {
    mReferenceVariables.push_back(variable);
  } else if (IType::isOwnerType(variable->getType())) {
    mOwnerVariables.push_back((IOwnerVariable*) variable);
  }
}

vector<string> Scope::getClearedVariables(map<string, IVariable *> allClearedVariables) {
  vector<string> variables;
  for (map<string, IVariable*>::iterator iterator = mVariables.begin();
       iterator != mVariables.end();
       iterator++) {
    string name = iterator->first;
    if (allClearedVariables.count(name)) {
      variables.push_back(name);
    }
  }
  return variables;
}

vector<IVariable*> Scope::getReferenceVariables() {
  return mReferenceVariables;
}

vector<IOwnerVariable*> Scope::getOwnerVariables() {
  return mOwnerVariables;
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

void Scope::setTryCatchInfo(TryCatchInfo* tryCatchInfo) {
  mTryCatchInfo = tryCatchInfo;
}

TryCatchInfo* Scope::getTryCatchInfo() {
  return mTryCatchInfo;
}

void Scope::clearTryCatchInfo() {
  assert(mTryCatchInfo != NULL);
  
  mTryCatchInfo = NULL;
}

void Scope::setObjectType(const IObjectType* objectType) {
  mObjectType = objectType;
}

const IObjectType* Scope::getObjectType() const {
  return mObjectType;
}

void Scope::setReturnType(const IType* type) {
  mReturnType = type;
}

const IType* Scope::getReturnType() {
  return mReturnType;
}

void Scope::freeOwnedMemory(IRGenerationContext& context,
                            map<string, IVariable*>& clearedVariables) {
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
    if (clearedVariables.count(iterator->first)) {
      continue;
    }
    IVariable* variable = iterator->second;
    if (IType::isOwnerType(variable->getType())) {
      ((IOwnerVariable*) variable)->free(context);
    }
  }
  
  mHasOwnedMemoryBeenFreed = true;
}

void Scope::addException(const Model* exception) {
  if (mExceptions.count(exception->getName())) {
    return;
  }
  mExceptions[exception->getName()] = exception;
}

void Scope::addExceptions(vector<const Model*> exceptions) {
  for (const Model* exception : exceptions) {
    addException(exception);
  }
}

void Scope::removeException(const Model* exception) {
  mExceptions.erase(exception->getName());
}

map<string, const Model*> Scope::getExceptions() {
  return mExceptions;
}
