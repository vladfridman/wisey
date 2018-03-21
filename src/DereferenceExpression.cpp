//
//  DereferenceExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/DereferenceExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/NativeType.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

DereferenceExpression::DereferenceExpression(IExpression* expression)
: mExpression(expression) { }

DereferenceExpression::~DereferenceExpression() {
  delete mExpression;
}

IVariable* DereferenceExpression::getVariable(IRGenerationContext& context,
                                              vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* DereferenceExpression::generateIR(IRGenerationContext& context,
                                         const IType* assignToType) const {
  Value* pointer = mExpression->generateIR(context, assignToType);
  return IRWriter::newLoadInst(context, pointer, "");
}

const IType* DereferenceExpression::getType(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  if (!expressionType->isNative()) {
    Log::e("Can not dereference expression of a non-native type");
    exit(1);
  }
  return ((ILLVMType*) expressionType)->getDereferenceType();
}

bool DereferenceExpression::isConstant() const {
  return false;
}

void DereferenceExpression::printToStream(IRGenerationContext& context,
                                          std::iostream& stream) const {
  stream << "::llvm::dereference(";
  mExpression->printToStream(context, stream);
  stream << ")";
}

