//
//  PrintErrStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "IntrinsicFunctions.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "PrintErrStatement.hpp"
#include "StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

PrintErrStatement::PrintErrStatement(IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

PrintErrStatement::~PrintErrStatement() {
  delete mExpression;
}

void PrintErrStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);
  IExpression::checkForUndefined(context, mExpression);

  const IType* expressionType = mExpression->getType(context);
  if (!StringType::isStringVariation(context, expressionType, mLine) &&
      (!expressionType->isPrimitive() || expressionType == PrimitiveTypes::VOID)) {
    context.reportError(mLine, "Argument in the printerr statement is not of printable type");
    throw 1;
  }
  ExpressionList expressions = IPrintStatement::getExpressions(context, mExpression, mLine);
  Value* formatPointer = IPrintStatement::getFormatString(context, expressions, mLine);

  GlobalVariable* stderrPointer = context.getModule()->getNamedGlobal(Names::getStdErrName());
  Value* stderrLoaded = IRWriter::newLoadInst(context, stderrPointer, "");
  
  Function* fprintf = IntrinsicFunctions::getFprintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(stderrLoaded);
  arguments.push_back(formatPointer);
  IPrintStatement::addPrintArguments(context, arguments, expressions, mLine);

  IRWriter::createCallInst(context, fprintf, arguments, "");
}
