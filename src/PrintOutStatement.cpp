//
//  PrintOutStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "IntrinsicFunctions.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"
#include "PrintOutStatement.hpp"
#include "StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

PrintOutStatement::PrintOutStatement(IExpression* expression, int line) :
mExpression(expression), mLine(line) {
}

PrintOutStatement::~PrintOutStatement() {
  delete mExpression;
}

void PrintOutStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);
  IExpression::checkForUndefined(context, mExpression);

  const IType* expressionType = mExpression->getType(context);
  if (!StringType::isStringVariation(context, expressionType, mLine) &&
      (!expressionType->isPrimitive() || expressionType == PrimitiveTypes::VOID)) {
    context.reportError(mLine, "Argument in the printout statement is not of printable type");
    throw 1;
  }
  ExpressionList expressions = IPrintStatement::getExpressions(context, mExpression, mLine);

  printExpressionList(context, expressions, mLine);
}

void PrintOutStatement::printExpressionList(IRGenerationContext& context,
                                            ExpressionList expressionList,
                                            int line) {
  Value* formatPointer = IPrintStatement::getFormatString(context, expressionList, line);
  Function* printf = IntrinsicFunctions::getPrintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(formatPointer);
  IPrintStatement::addPrintArguments(context, arguments, expressionList, line);
  
  IRWriter::createCallInst(context, printf, arguments, "");
}
