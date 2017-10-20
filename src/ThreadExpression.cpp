//
//  ThreadExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string ThreadExpression::THREAD = "thread";

IVariable* ThreadExpression::getVariable(IRGenerationContext& context) const {
  return context.getScopes().getVariable(THREAD);
}

Value* ThreadExpression::generateIR(IRGenerationContext& context) const {
  return IVariable::getVariable(context, THREAD)->generateIdentifierIR(context, "");
}

const IType* ThreadExpression::getType(IRGenerationContext& context) const {
  return IVariable::getVariable(context, THREAD)->getType();
}

void ThreadExpression::releaseOwnership(IRGenerationContext& context) const {
  // thread expression is always avialible, it is neither freed nor cleared
}

void ThreadExpression::addReferenceToOwner(IRGenerationContext& context,
                                           IVariable* reference) const {
  Log::e("Can not store a reference to the current thread");
  exit(1);
}

bool ThreadExpression::existsInOuterScope(IRGenerationContext& context) const {
  return IVariable::getVariable(context, THREAD)->existsInOuterScope();
}

bool ThreadExpression::isConstant() const {
  return false;
}

void ThreadExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << THREAD;
}

