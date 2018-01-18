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

NegateExpression::NegateExpression(IExpression* expression) : mExpression(expression) { }

NegateExpression::~NegateExpression() {
  delete mExpression;
}

IVariable* NegateExpression::getVariable(IRGenerationContext& context,
                                         vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* NegateExpression::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  assert(flag == IR_GENERATION_NORMAL);
  
  const IType* type = getType(context);
  if (!IType::isPrimitveType(type) || type == PrimitiveTypes::VOID_TYPE) {
    Log::e("Can not apply negate operation to type '" + type->getTypeName() + "'");
    exit(1);
  }
  
  if (type == PrimitiveTypes::FLOAT_TYPE || type == PrimitiveTypes::DOUBLE_TYPE) {
    Value* zero = ConstantFP::get(type->getLLVMType(context), 0);
    return IRWriter::createBinaryOperator(context,
                                          Instruction::FSub,
                                          zero,
                                          mExpression->generateIR(context, flag),
                                          "fsub");
  }
  
  Value* zero = ConstantInt::get(type->getLLVMType(context), 0);
  return IRWriter::createBinaryOperator(context,
                                        Instruction::Sub,
                                        zero,
                                        mExpression->generateIR(context, flag),
                                        "sub");
}

const IType* NegateExpression::getType(IRGenerationContext& context) const {
  return mExpression->getType(context);
}

bool NegateExpression::isConstant() const {
  return false;
}

void NegateExpression::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "!";
  mExpression->printToStream(context, stream);
}
