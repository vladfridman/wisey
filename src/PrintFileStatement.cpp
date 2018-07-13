//
//  PrintFileStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/2/18.
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
#include "wisey/PrintFileStatement.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

PrintFileStatement::PrintFileStatement(IExpression* fileExpression,
                                       IExpression* expression,
                                       int line) :
mFileExpression(fileExpression), mExpression(expression), mLine(line) {
}

PrintFileStatement::~PrintFileStatement() {
  delete mFileExpression;
  delete mExpression;
}

void PrintFileStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);
  
  Controller* fileController = context.getController(Names::getFileControllerFullName(), mLine);
  assert(fileController && "wisey.io.CFile is not defined");
  const IType* fileExpressionType = mFileExpression->getType(context);
  if (fileExpressionType != fileController && fileExpressionType != fileController->getOwner()) {
    context.reportError(mLine, "First argument to the printerr instruction should be "
                        "an instance of " + Names::getFileControllerFullName());
    throw 1;
  }
  
  Value* fileExpressionValue = mFileExpression->generateIR(context, PrimitiveTypes::VOID);
  FakeExpression* fakeIdentifier = new FakeExpression(fileExpressionValue, fileExpressionType);
  IdentifierChain* identifierChain = new IdentifierChain(fakeIdentifier,
                                                         Names::getFileStructMethodName(),
                                                         mLine);
  ExpressionList methodCallArguments;
  MethodCall* methodCall = MethodCall::create(identifierChain, methodCallArguments, mLine);
  Value* fileStruct = methodCall->generateIR(context, PrimitiveTypes::VOID);
  const IType* expressionType = mExpression->getType(context);
  if (!StringType::isStringVariation(context, expressionType, mLine) &&
      (!expressionType->isPrimitive() || expressionType == PrimitiveTypes::VOID)) {
    context.reportError(mLine, "Argument in the printfile statement is not of printable type");
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

