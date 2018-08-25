//
//  DereferenceExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/DereferenceExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPointerType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

DereferenceExpression::DereferenceExpression(const IExpression* expression, int line) :
mExpression(expression), mLine(line) {
}

DereferenceExpression::~DereferenceExpression() {
  delete mExpression;
}

int DereferenceExpression::getLine() const {
  return mLine;
}

Value* DereferenceExpression::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  checkExpression(context);
  
  Value* expressionValue = mExpression->generateIR(context, PrimitiveTypes::VOID);
  assert(expressionValue->getType()->isPointerTy() && "Pointer type expected");
  
  return IRWriter::newLoadInst(context, expressionValue, "");
}

const ILLVMType* DereferenceExpression::getType(IRGenerationContext& context) const {
  checkExpression(context);
  
  return ((const LLVMPointerType*) mExpression->getType(context))->getBaseType();
}

bool DereferenceExpression::isConstant() const {
  return false;
}

bool DereferenceExpression::isAssignable() const {
  return false;
}

void DereferenceExpression::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "::llvm::dereference(";
  mExpression->printToStream(context, stream);
  stream << ")";
}

void DereferenceExpression::checkExpression(IRGenerationContext& context) const {
  const IType* type = mExpression->getType(context);
  if (type->isPointer()) {
    return;
  }
  
  context.reportError(mLine, "Argument in the ::llvm::dereference must be of llvm pointer type");
  throw 1;
}
