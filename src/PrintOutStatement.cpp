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

PrintOutStatement::PrintOutStatement(ExpressionList expressionList, int line) :
mExpressionList(expressionList), mLine(line) {
}

PrintOutStatement::~PrintOutStatement() {
  for (const IExpression* expression : mExpressionList) {
    delete expression;
  }
  mExpressionList.clear();
}

void PrintOutStatement::generateIR(IRGenerationContext& context) const {
  Value* formatPointer = IPrintStatement::getFormatString(context, mExpressionList, mLine);

  Function* printf = IntrinsicFunctions::getPrintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(formatPointer);
  for (const IExpression* expression : mExpressionList) {
    arguments.push_back(expression->generateIR(context, PrimitiveTypes::VOID_TYPE));
  }
  IRWriter::createCallInst(context, printf, arguments, "");
}
