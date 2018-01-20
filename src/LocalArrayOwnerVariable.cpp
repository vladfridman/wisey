//
//  LocalArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalArrayOwnerVariable::LocalArrayOwnerVariable(string name,
                                                 const ArrayOwnerType* type,
                                                 llvm::Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

LocalArrayOwnerVariable::~LocalArrayOwnerVariable() {
}

string LocalArrayOwnerVariable::getName() const {
  return mName;
}

const wisey::ArrayOwnerType* LocalArrayOwnerVariable::getType() const {
  return mType;
}

llvm::Value* LocalArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

llvm::Value* LocalArrayOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                           IExpression* assignToExpression,
                                                           vector<const IExpression*> arrayIndices,
                                                           int line) {
  if (!arrayIndices.size()) {
    return generateWholeArrayAssignment(context, assignToExpression, line);
  }
  
  Value* arrayPointer = IRWriter::newLoadInst(context, mValueStore, "");
  const IType* scalarType = mType->getArrayType()->getScalarType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  mType->getArrayType(),
                                                                  arrayPointer,
                                                                  arrayIndices);
  
  return ArrayElementAssignment::generateElementAssignment(context,
                                                           scalarType,
                                                           assignToExpression,
                                                           elementStore,
                                                           line);
}

llvm::Value* LocalArrayOwnerVariable::generateWholeArrayAssignment(IRGenerationContext& context,
                                                                   IExpression* assignToExpression,
                                                                   int line) {
  const IType* assignToType = assignToExpression->getType(context);
  if (assignToType != mType) {
    Log::e("Incompatable array types in array assignement");
    exit(1);
  }
  
  free(context);

  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_RELEASE);
  IRWriter::newStoreInst(context, assignToValue, mValueStore);
  
  return assignToValue;
}

void LocalArrayOwnerVariable::setToNull(IRGenerationContext& context) {
  Value* null = ConstantPointerNull::get(mType->getLLVMType(context)->getPointerTo());
  IRWriter::newStoreInst(context, null, mValueStore);
}

void LocalArrayOwnerVariable::free(IRGenerationContext& context) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, value);
}
