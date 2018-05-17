//
//  LLVMFunctionCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/LLVMFunctionCall.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMFunctionCall::LLVMFunctionCall(string functionName, ExpressionList arguments, int line) :
mFunctionName(functionName), mArguments(arguments), mLine(line) {
}

LLVMFunctionCall::~LLVMFunctionCall() {
}

int LLVMFunctionCall::getLine() const {
  return mLine;
}

Value* LLVMFunctionCall::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  Function* function = context.getModule()->getFunction(mFunctionName);
  const LLVMFunctionType* functionType = context.lookupLLVMFunctionNamedType(mFunctionName, mLine);
  if (!function || !functionType) {
    context.reportError(mLine, "LLVM function " + mFunctionName + " is not defined");
    throw 1;
  }
  
  vector<Value*> arguments;
  vector<const IType*> argumentTypes = functionType->getArgumentTypes();
  auto argumentTypesIterator = argumentTypes.begin();
  for (const IExpression* argumentExpression : mArguments) {
    Value* expressionValue = argumentExpression->generateIR(context, PrimitiveTypes::VOID);
    if (argumentTypesIterator == argumentTypes.end()) {
      arguments.push_back(expressionValue);
      continue;
    }

    const IType* expressionType = argumentExpression->getType(context);
    Value* castValue = AutoCast::maybeCast(context,
                                           expressionType,
                                           expressionValue,
                                           *argumentTypesIterator,
                                           mLine);
    arguments.push_back(castValue);
    argumentTypesIterator++;
  }
  return IRWriter::createCallInst(context, function, arguments, "");
}

const IType* LLVMFunctionCall::getType(IRGenerationContext& context) const {
  Function* function = context.getModule()->getFunction(mFunctionName);
  if (function == NULL) {
    context.reportError(mLine, "LLVM function " + mFunctionName + " is not defined");
    throw 1;
  }
  
  return context.lookupLLVMFunctionNamedType(mFunctionName, mLine)->getReturnType();
}

bool LLVMFunctionCall::isConstant() const {
  return false;
}

bool LLVMFunctionCall::isAssignable() const {
  return false;
}

void LLVMFunctionCall::printToStream(IRGenerationContext& context, iostream& stream) const {
  assert(false);
}
