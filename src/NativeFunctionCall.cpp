//
//  NativeFunctionCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRWriter.hpp"
#include "wisey/NativeFunctionCall.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NativeFunctionCall::NativeFunctionCall(Function* function, vector<IExpression*> arguments) :
mFunction(function),
mArguments(arguments) {
}

NativeFunctionCall::~NativeFunctionCall() {
  for (IExpression* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
}

Value* NativeFunctionCall::generateIR(IRGenerationContext& context) const {
  vector<Value*> arguments;
  for (IExpression* argumentExpression : mArguments) {
    Value* value = argumentExpression->generateIR(context, PrimitiveTypes::VOID_TYPE);
    arguments.push_back(value);
  }
  return IRWriter::createCallInst(context, mFunction, arguments, "");
}
