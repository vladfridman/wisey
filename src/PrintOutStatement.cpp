//
//  PrintOutStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"

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
  
  const IType* expressionType = mExpression->getType(context);
  if (!isCharArray(context, expressionType, mLine) &&
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
  for (const IExpression* expression : expressionList) {
    const IType* expressionType = expression->getType(context);
    Value* expressionValue = expression->generateIR(context, PrimitiveTypes::VOID);
    if (isCharArray(context, expressionType, line)) {
      arguments.push_back(ArrayType::extractLLVMArray(context, expressionValue));
    } else {
      arguments.push_back(expressionValue);
    }
  }
  IRWriter::createCallInst(context, printf, arguments, "");
}
