//
//  Scopes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <set>

#include "wisey/Catch.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/IOwnerVariable.hpp"
#include "wisey/IReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/Names.hpp"
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

void Scopes::clearVariable(IRGenerationContext& context, string name) {
  if (mScopes.size() == 0) {
    Log::e("Could not clear variable '" + name + "': the Scopes stack is empty");
    exit(1);
    return;
  }
  
  for (Scope* scope : mScopes) {
    IVariable* variable = scope->findVariable(name);
    if (variable == NULL) {
      continue;
    }
    if (IType::isOwnerType(variable->getType())) {
      clearReferencesToOwnerTypeVariable(variable);
      ((IOwnerVariable*) variable)->setToNull(context);
    }
    mClearedVariables[name] = variable;
    return;
  }
  
  Log::e("Could not clear variable '" + name + "': it was not found");
  exit(1);
  return;
}

void Scopes::setVariable(IVariable* variable) {
  if (getVariable(variable->getName())) {
    Log::e("Already declared variable named '" + variable->getName() +
           "'. Variable hiding is not allowed.");
    exit(1);
  }
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

bool Scopes::isVariableCleared(string name) {
  return mClearedVariables.count(name);
}

void Scopes::pushScope() {
  mScopes.push_front(new Scope());
}

void Scopes::popScope(IRGenerationContext& context) {
  Scope* top = mScopes.front();

  top->freeOwnedMemory(context, mClearedVariables);

  for (string variableName : top->getClearedVariables(mClearedVariables)) {
    mClearedVariables.erase(variableName);
  }
  for (IOwnerVariable* owner : top->getOwnerVariables()) {
    map<string, IVariable*> references = mOwnerToReferencesMap[owner->getName()];
    for (map<string, IVariable*>::iterator iterator = references.begin();
         iterator != references.end();
         iterator++) {
      IVariable* reference = iterator->second;
      mRererenceToOwnersMap[reference->getName()].erase(owner->getName());
    }
    mOwnerToReferencesMap.erase(owner->getName());
  }
  for (IReferenceVariable* variable : top->getReferenceVariables()) {
    if (!mRererenceToOwnersMap.count(variable->getName())) {
      continue;
    }
    map<string, IVariable*> owners = mRererenceToOwnersMap.at(variable->getName());
    for (map<string, IVariable*>::iterator iterator = owners.begin();
         iterator != owners.end();
         iterator++) {
      IVariable* owner = iterator->second;
      mOwnerToReferencesMap[owner->getName()].erase(variable->getName());
    }
    mRererenceToOwnersMap.erase(variable->getName());
  }
  
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
    scope->freeOwnedMemory(context, mClearedVariables);
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

vector<Catch*> Scopes::mergeNestedCatchLists(IRGenerationContext& context,
                                             vector<Catch*> catchList) {
  vector<Catch*> result;
  set<string> processedCatches;
  
  for(Catch* catchClause : catchList) {
    result.push_back(catchClause);
    processedCatches.insert(catchClause->getType(context)->getName());
  }
  
  for (Scope* scope : mScopes) {
    TryCatchInfo* info = scope->getTryCatchInfo();
    if (info == NULL) {
      continue;
    }
    for (Catch* catchClause : info->getCatchList()) {
      string typeName = catchClause->getType(context)->getName();
      if (processedCatches.find(typeName) == processedCatches.end()) {
        result.push_back(catchClause);
        processedCatches.insert(typeName);
      }
    }
  }
  
  return result;
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

void Scopes::clearReferencesToOwnerTypeVariable(IVariable* ownerVariable) {
  if (!mOwnerToReferencesMap.count(ownerVariable->getName())) {
    return;
  }

  map<string, IVariable*> referencedVariables = mOwnerToReferencesMap.at(ownerVariable->getName());
  for (map<string, IVariable*>::iterator iterator = referencedVariables.begin();
       iterator != referencedVariables.end();
       iterator++) {
    IVariable* referenceVariable = iterator->second;
    mClearedVariables[referenceVariable->getName()] = referenceVariable;
  }
}

void Scopes::addReferenceToOwnerVariable(IVariable* ownerVariable, IVariable* referenceVariable) {
  mOwnerToReferencesMap[ownerVariable->getName()][referenceVariable->getName()] = referenceVariable;
  mRererenceToOwnersMap[referenceVariable->getName()][ownerVariable->getName()] = ownerVariable;
}

map<string, IVariable*> Scopes::getOwnersForReference(IVariable* reference) {
  return mRererenceToOwnersMap[reference->getName()];
}
