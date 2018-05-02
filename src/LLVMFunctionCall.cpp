//
//  LLVMFunctionCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

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
  if (function == NULL) {
    Log::e_deprecated("LLVM function " + mFunctionName + " is not defined");
    exit(1);
  }
  
  vector<Value*> arguments;
  for (const IExpression* argumentExpression : mArguments) {
    arguments.push_back(argumentExpression->generateIR(context, PrimitiveTypes::VOID));
  }
  return IRWriter::createCallInst(context, function, arguments, "");
}

const IType* LLVMFunctionCall::getType(IRGenerationContext& context) const {
  Function* function = context.getModule()->getFunction(mFunctionName);
  if (function == NULL) {
    Log::e_deprecated("LLVM function " + mFunctionName + " is not defined");
    exit(1);
  }
  
  return context.lookupLLVMFunctionNamedType(mFunctionName)->getReturnType();
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
