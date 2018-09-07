//
//  PrintStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/27/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintStatement.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

PrintStatement::PrintStatement(IExpression* streamExpression,
                               IExpression* expression,
                               int line) :
mStreamExpression(streamExpression), mExpression(expression), mLine(line) {
}

PrintStatement::~PrintStatement() {
  delete mStreamExpression;
  delete mExpression;
}

void PrintStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);
  IExpression::checkForUndefined(context, mStreamExpression);
  IExpression::checkForUndefined(context, mExpression);

  Controller* streamController = context.getController(Names::getTextStreamControllerFullName(),
                                                       mLine);
  assert(streamController && "wisey.io.CTextStreamWriter is not defined");
  const IType* streamExpressionType = mStreamExpression->getType(context);
  if (streamExpressionType != streamController &&
      streamExpressionType != streamController->getOwner()) {
    context.reportError(mLine, "First argument to the print instruction should be "
                        "an instance of " + Names::getTextStreamControllerFullName());
    throw 1;
  }
  
  Value* streamExpressionValue = mStreamExpression->generateIR(context, PrimitiveTypes::VOID);
  FakeExpression* fakeIdentifier = new FakeExpression(streamExpressionValue, streamExpressionType);
  IdentifierChain* identifierChain = new IdentifierChain(fakeIdentifier,
                                                         Names::getStreamMethodName(),
                                                         mLine);
  ExpressionList methodCallArguments;
  MethodCall* methodCall = MethodCall::create(identifierChain, methodCallArguments, mLine);
  Value* fileStruct = methodCall->generateIR(context, PrimitiveTypes::VOID);
  const IType* expressionType = mExpression->getType(context);
  if (!StringType::isStringVariation(context, expressionType, mLine) &&
      (!expressionType->isPrimitive() || expressionType == PrimitiveTypes::VOID)) {
    context.reportError(mLine, "Argument in the print statement is not of printable type");
    throw 1;
  }
  ExpressionList expressions = IPrintStatement::getExpressions(context, mExpression, mLine);
  Value* formatPointer = IPrintStatement::getFormatString(context, expressions, mLine);
  
  Function* fprintf = IntrinsicFunctions::getFprintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(fileStruct);
  arguments.push_back(formatPointer);
  IPrintStatement::addPrintArguments(context, arguments, expressions, mLine);
  
  IRWriter::createCallInst(context, fprintf, arguments, "");
  
  delete methodCall;
}

