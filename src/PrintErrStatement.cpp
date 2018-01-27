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

PrintErrStatement::PrintErrStatement(ExpressionList expressionList) :
mExpressionList(expressionList) { }

PrintErrStatement::~PrintErrStatement() {
  for (const IExpression* expression : mExpressionList) {
    delete expression;
  }
  mExpressionList.clear();
}

Value* PrintErrStatement::generateIR(IRGenerationContext& context) const {
  Value* formatPointer = IPrintStatement::getFormatString(context, mExpressionList);

  GlobalVariable* stderrPointer = context.getModule()->getNamedGlobal(Names::getStdErrName());
  Value* stderrLoaded = IRWriter::newLoadInst(context, stderrPointer, "");
  
  Function* fprintf = IntrinsicFunctions::getFprintfFunction(context);
  vector<Value*> arguments;
  arguments.push_back(stderrLoaded);
  arguments.push_back(formatPointer);
  for (const IExpression* expression : mExpressionList) {
    arguments.push_back(expression->generateIR(context, PrimitiveTypes::VOID_TYPE));
  }
  
  Value* pointer = IRWriter::newAllocaInst(context, stderrLoaded->getType(), "");
  IRWriter::newStoreInst(context, stderrLoaded, pointer);
  
  return IRWriter::createCallInst(context, fprintf, arguments, "");
}
