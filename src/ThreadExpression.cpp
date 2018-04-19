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

string ThreadExpression::CALL_STACK = "callstack";

ThreadExpression::ThreadExpression(int line) : mLine(line) { }

ThreadExpression::~ThreadExpression() { }

int ThreadExpression::getLine() const {
  return mLine;
}

IVariable* ThreadExpression::getVariable(IRGenerationContext& context,
                                         vector<const IExpression*>& arrayIndices) const {
  return context.getScopes().getVariable(THREAD);
}

Value* ThreadExpression::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  return IVariable::getVariable(context, THREAD)->generateIdentifierIR(context);
}

const IType* ThreadExpression::getType(IRGenerationContext& context) const {
  return IVariable::getVariable(context, THREAD)->getType();
}

bool ThreadExpression::isConstant() const {
  return false;
}

bool ThreadExpression::isAssignable() const {
  return true;
}

void ThreadExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << THREAD;
}

