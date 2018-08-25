//
//  NegateExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/NegateExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NegateExpression::NegateExpression(const IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

NegateExpression::~NegateExpression() {
  delete mExpression;
}

int NegateExpression::getLine() const {
  return mLine;
}

Value* NegateExpression::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IType* type = getType(context);
  if (!type->isPrimitive() || type == PrimitiveTypes::VOID) {
    context.reportError(mLine,
                        "Can not apply negate operation to type '" + type->getTypeName() + "'");
    throw 1;
  }
  
  if (type == PrimitiveTypes::FLOAT || type == PrimitiveTypes::DOUBLE) {
    Value* zero = ConstantFP::get(type->getLLVMType(context), 0);
    return IRWriter::createBinaryOperator(context,
                                          Instruction::FSub,
                                          zero,
                                          mExpression->generateIR(context, assignToType),
                                          "fsub");
  }
  
  Value* zero = ConstantInt::get(type->getLLVMType(context), 0);
  return IRWriter::createBinaryOperator(context,
                                        Instruction::Sub,
                                        zero,
                                        mExpression->generateIR(context, assignToType),
                                        "sub");
}

const IType* NegateExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool NegateExpression::isConstant() const {
  return false;
}

bool NegateExpression::isAssignable() const {
  return false;
}

void NegateExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "!";
  mExpression->printToStream(context, stream);
}
