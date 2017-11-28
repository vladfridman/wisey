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

ThreadExpression::ThreadExpression() { }

ThreadExpression::~ThreadExpression() { }

IVariable* ThreadExpression::getVariable(IRGenerationContext& context) const {
  return context.getScopes().getVariable(THREAD);
}

Value* ThreadExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  return IVariable::getVariable(context, THREAD)->generateIdentifierIR(context);
}

const IType* ThreadExpression::getType(IRGenerationContext& context) const {
  return IVariable::getVariable(context, THREAD)->getType();
}

bool ThreadExpression::isConstant() const {
  return false;
}

void ThreadExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << THREAD;
}

