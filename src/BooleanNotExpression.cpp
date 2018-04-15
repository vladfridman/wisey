//
//  BooleanNotExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>

#include "wisey/BooleanNotExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BooleanNotExpression::BooleanNotExpression(IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

BooleanNotExpression::~BooleanNotExpression() {
  delete mExpression;
}

int BooleanNotExpression::getLine() const {
  return mLine;
}

IVariable* BooleanNotExpression::getVariable(IRGenerationContext& context,
                                             vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* BooleanNotExpression::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  const IType* expressionType = mExpression->getType(context);
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE) {
    Log::e(context.getImportProfile(),
           mLine,
           "Boolean NOT operator '!' can only be applied to boolean types");
    exit(1);
  }
  
  Value* one = ConstantInt::get(Type::getInt1Ty(context.getLLVMContext()), 1);
  Value* expressionValue = mExpression->generateIR(context, assignToType);
  return IRWriter::createBinaryOperator(context, Instruction::Xor, expressionValue, one, "lnot");
}

const IType* BooleanNotExpression::getType(IRGenerationContext& context) const {
  return PrimitiveTypes::BOOLEAN_TYPE;
}

bool BooleanNotExpression::isConstant() const {
  return false;
}

void BooleanNotExpression::printToStream(IRGenerationContext& context,
                                         std::iostream& stream) const {
  stream << "!";
  mExpression->printToStream(context, stream);
}
