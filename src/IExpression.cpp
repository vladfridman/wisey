//
//  IExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "UndefinedType.hpp"

using namespace std;
using namespace wisey;

void IExpression::requireAnnotated(IRGenerationContext& context,
                                   const IExpression* expression,
                                   const char* consumerDescription) {
  if (!expression->isCallExpression()) {
    return;
  }
  if (expression->getLine() <= 0) {
    return;
  }
  context.reportError(expression->getLine(),
                      string("Call expression in ") + consumerDescription +
                      " must be annotated with `expr -> Type`. "
                      "Add the result type at the call site so the type is visible "
                      "without resolving the callee.");
  throw 1;
}

int IExpression::countMethodChainDepth(const IExpression* methodCall) {
  int depth = 0;
  const IExpression* cur = methodCall;
  while (cur && cur->isCallExpression()) {
    depth++;
    const IExpression* receiver = cur->getCallReceiver();
    if (!receiver) {
      break;
    }
    const IExpression* under = receiver->peelDotReceiver();
    cur = under;
  }
  return depth;
}

void IExpression::requireShortMethodChain(IRGenerationContext& context,
                                          const IExpression* methodCall) {
  if (methodCall->getLine() <= 0) {
    return;
  }
  int depth = countMethodChainDepth(methodCall);
  if (depth < 3) {
    return;
  }
  context.reportError(methodCall->getLine(),
                      string("Method chain of depth ") + std::to_string(depth) +
                      " is too deep. Break it into named locals so each "
                      "intermediate type is visible. Builder chains "
                      "(build/inject/create with .with* and .onHeap/onPool) "
                      "are exempt.");
  throw 1;
}

void IExpression::checkForUndefined(IRGenerationContext& context,
                                    const IExpression* expression) {
  const IType* expressionType = expression->getType(context);
  if (expressionType != UndefinedType::UNDEFINED && !expressionType->isPackage()) {
    return;
  }

  stringstream stringStream;
  expression->printToStream(context, stringStream);
  context.reportError(expression->getLine(), "Undefined expression '" + stringStream.str() + "'");
  throw 1;
}
