//
//  PrintErrStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintErrStatement.hpp"

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
  
  const IType* expressionType = mExpression->getType(context);
  if (!expressionType->isPrimitive() || expressionType == PrimitiveTypes::VOID) {
    context.reportError(mLine, "Argument in the printerr statement is not of printable type");
    exit(1);
  }
  ExpressionList expressions = IPrintStatement::getExpressions(context, mExpression, mLine);
  Value* formatPointer = IPrintStatement::getFormatString(context, expressions, mLine);

  GlobalVariable* stderrPointer = context.getModule()->getNamedGlobal(Names::getStdErrName());
  Value* stderrLoaded = IRWriter::newLoadInst(context, stderrPointer, "");
  
  Function* fprintf = IntrinsicFunctions::getFprintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(stderrLoaded);
  arguments.push_back(formatPointer);
  for (const IExpression* expression : expressions) {
    arguments.push_back(expression->generateIR(context, PrimitiveTypes::VOID));
  }
  
  IRWriter::createCallInst(context, fprintf, arguments, "");
}
