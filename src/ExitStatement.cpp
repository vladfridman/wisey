//
//  ExitStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExitStatement.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;


ExitStatement::ExitStatement(IExpression* expression) : mExpression(expression) { }

ExitStatement::~ExitStatement() {
  delete mExpression;
}

Value* ExitStatement::generateIR(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  if (!expressionType->canAutoCastTo(PrimitiveTypes::INT_TYPE)) {
    Log::e("Can not auto cast exit statement parameter to int");
    exit(1);
  }
  Value* expressionValue = mExpression->generateIR(context);
  Value* castValue = expressionType->castTo(context, expressionValue, PrimitiveTypes::INT_TYPE);

  Function* exitFunction = IntrinsicFunctions::getExitFunction(context);
  vector<Value*> arguments;
  arguments.push_back(castValue);
  IRWriter::createCallInst(context, exitFunction, arguments, "");
  return IRWriter::newUnreachableInst(context);
}
