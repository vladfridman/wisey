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
#include "PrimitiveTypes.hpp"
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

void IExpression::requireReceiverAnnotated(IRGenerationContext& context,
                                           const IExpression* methodCall,
                                           const IExpression* dottedReceiver) {
  // Synthesized libwisey AST nodes have line <= 0; skip them, same gate as
  // requireAnnotated. Without it every header-parsed call would crash here.
  if (methodCall->getLine() <= 0) {
    return;
  }
  // Direct `foo()` calls (no dot) parse with a non-IdentifierChain receiver,
  // and IdentifierChain::peelDotReceiver returns the LHS-of-dot. Anything
  // outside that shape isn't a `.method(...)` call and needs no annotation.
  if (dottedReceiver == nullptr) {
    return;
  }
  // The compiler synthesizes IdentifierChain+MethodCall internally to call
  // helper methods like `getContent` (StringType::callGetContent) and array
  // ops. The synthesized receiver is a FakeExpression with line 0 even
  // though the enclosing MethodCall carries the user's source line. Skip
  // enforcement when the receiver itself is line-zero — it's compiler-
  // generated, not user code.
  if (dottedReceiver->getLine() <= 0) {
    return;
  }
  if (dottedReceiver->isReceiverAnnotation()) {
    return;
  }
  // Exempt `this.method(...)` — the enclosing type is right there in the file
  // and re-annotating it on every method call is pure boilerplate.
  if (dottedReceiver->isThisIdentifier()) {
    return;
  }
  // Exempt array-getSize and string-getLength — these are the only methods on
  // those types and their receiver type is unambiguous from context. Object
  // types are the only place receiver annotation pulls its weight.
  const IType* receiverType = dottedReceiver->getType(context);
  if (receiverType->isArray() || receiverType == PrimitiveTypes::STRING) {
    return;
  }

  std::string fullTypeName = receiverType->getTypeName();
  std::stringstream printer;
  dottedReceiver->printToStream(context, printer);
  // Suggest the full-path form (e.g. `wisey.threads.CCallStack`). The grammar
  // accepts short, package-qualified, and inner-type forms; we suggest full
  // so the migration is unambiguous regardless of which package the file
  // lives in. Form is `recv:Type:method(...)` — colons frame the type so the
  // `.` between `Outer.Inner` doesn't collide with the method-name `.`.
  context.reportError(methodCall->getLine(),
                      "Method-call receiver '" + printer.str() +
                      "' missing type annotation. " +
                      "Suggested: '" + printer.str() + ":" +
                      fullTypeName + ":method(...)'. " +
                      "The annotation surfaces the receiver type at the call site so a "
                      "reader can verify the method exists without resolving the receiver.");
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
