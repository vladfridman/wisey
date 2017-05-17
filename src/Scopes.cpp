//
//  Scopes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <set>

#include "wisey/EmptyStatement.hpp"
#include "wisey/IObjectWithMethodsType.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

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

void Scopes::clearVariable(string name) {
  if (mScopes.size() == 0) {
    Log::e("Could not clear variable '" + name + "': the Scopes stack is empty");
    exit(1);
    return;
  }
  
  for (Scope* scope : mScopes) {
    IVariable* variable = scope->findVariable(name);
    if (variable != NULL) {
      scope->clearVariable(name);
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

void Scopes::popScope(IRGenerationContext& context) {
  Scope* top = mScopes.front();
  top->maybeFreeOwnedMemory(context);
  map<string, IType*> exceptions = top->getExceptions();
  mScopes.pop_front();
  delete top;
  
  if (exceptions.size() == 0) {
    return;
  }
  
  if (mScopes.size() == 0) {
    reportUnhandledExceptions(exceptions);
  }
  
  top = mScopes.front();
  for (map<string, IType*>::iterator iterator = exceptions.begin();
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

void Scopes::setLandingPadBlock(BasicBlock* basicBlock) {
  getScope()->setLandingPadBlock(basicBlock);
}

BasicBlock* Scopes::getLandingPadBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    BasicBlock* block= scope->getLandingPadBlock();
    if (block != NULL) {
      return block;
    }
  }
  
  return NULL;
}

void Scopes::setExceptionContinueBlock(BasicBlock* basicBlock) {
  getScope()->setExceptionContinueBlock(basicBlock);
}

BasicBlock* Scopes::getExceptionContinueBlock() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    BasicBlock* block= scope->getExceptionContinueBlock();
    if (block != NULL) {
      return block;
    }
  }
  
  return NULL;
}

void Scopes::setExceptionFinally(const IStatement* finallyStatement) {
  getScope()->setExceptionFinally(finallyStatement);
}

const IStatement* Scopes::getExceptionFinally() {
  if (mScopes.size() == 0) {
    return &EmptyStatement::EMPTY_STATEMENT;
  }
  
  for (Scope* scope : mScopes) {
    const IStatement* finallyStatement = scope->getExceptionFinally();
    if (finallyStatement != NULL) {
      return finallyStatement;
    }
  }
  
  return &EmptyStatement::EMPTY_STATEMENT;
}

void Scopes::setReturnType(IType* type) {
  getScope()->setReturnType(type);
}

IType* Scopes::getReturnType() {
  if (mScopes.size() == 0) {
    return NULL;
  }
  
  for (Scope* scope : mScopes) {
    IType* returnType = scope->getReturnType();
    if (returnType != NULL) {
      return returnType;
    }
  }
  
  return NULL;
}

void Scopes::reportUnhandledExceptions(map<string, IType*> exceptions) {
  for (map<string, IType*>::iterator iterator = exceptions.begin();
       iterator != exceptions.end();
       iterator++) {
    Log::e("Exception " + iterator->first + " is not handled");
  }
  exit(1);
}
