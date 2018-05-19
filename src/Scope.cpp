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

Scope::Scope() :
mBreakToBlock(NULL),
mContinueToBlock(NULL),
mTryCatchInfo(NULL),
mReturnType(NULL) { }

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
  const IType* variableType = variable->getType();
  if (variableType->isReference() && !variable->isSystem()) {
    mReferenceVariables.push_front((IReferenceVariable*) variable);
  } else if (variableType->isOwner() && !variable->isField()) {
    mOwnerVariables.push_front((IOwnerVariable*) variable);
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

void Scope::setReturnType(const IType* type) {
  mReturnType = type;
}

const IType* Scope::getReturnType() {
  return mReturnType;
}

void Scope::freeOwnedMemory(IRGenerationContext& context, Value* exception, int line) {
  BasicBlock* currentBlock = context.getBasicBlock();
  if(currentBlock != NULL &&
     currentBlock->size() > 0 &&
     TerminatorInst::classof(&currentBlock->back())) {
    return;
  }
  
  for (IReferenceVariable* referenceVariable : mReferenceVariables) {
    referenceVariable->decrementReferenceCounter(context);
  }
  
  if (!mOwnerVariables.size()) {
    return;
  }
  
  for (IOwnerVariable* ownerVariable : mOwnerVariables) {
    ownerVariable->free(context, exception, line);
  }
}

void Scope::addException(const Model* exception, int line) {
  mExceptions[exception->getTypeName()] = line;
}

void Scope::addExceptions(vector<const Model*> exceptions, int line) {
  for (const Model* exception : exceptions) {
    addException(exception, line);
  }
}

void Scope::removeException(const Model* exception) {
  mExceptions.erase(exception->getTypeName());
}

map<string, int> Scope::getExceptions() {
  return mExceptions;
}
