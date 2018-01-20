//
//  LocalArrayReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayReferenceVariable.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalArrayReferenceVariable::LocalArrayReferenceVariable(string name,
                                                         const ArrayType* type,
                                                         llvm::Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

LocalArrayReferenceVariable::~LocalArrayReferenceVariable() {
}

string LocalArrayReferenceVariable::getName() const {
  return mName;
}

const wisey::ArrayType* LocalArrayReferenceVariable::getType() const {
  return mType;
}

llvm::Value* LocalArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

llvm::Value* LocalArrayReferenceVariable::
generateAssignmentIR(IRGenerationContext& context,
                     IExpression* assignToExpression,
                     vector<const IExpression*> arrayIndices,
                     int line) {
  if (!arrayIndices.size()) {
    return generateWholeArrayAssignment(context, assignToExpression, line);
  }
  
  Value* arrayPointer = IRWriter::newLoadInst(context, mValueStore, "");
  const IType* scalarType = mType->getScalarType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  mType,
                                                                  arrayPointer,
                                                                  arrayIndices);
  
  return ArrayElementAssignment::generateElementAssignment(context,
                                                           scalarType,
                                                           assignToExpression,
                                                           elementStore,
                                                           line);
}

llvm::Value* LocalArrayReferenceVariable::
generateWholeArrayAssignment(IRGenerationContext& context,
                             IExpression* assignToExpression,
                             int line) {
  const IType* assignToType = assignToExpression->getType(context);
  if (assignToType->getTypeKind() == ARRAY_OWNER_TYPE) {
    assignToType = ((const ArrayOwnerType*) assignToType)->getArrayType();
  }
  
  if (assignToType != mType) {
    Log::e("Incompatable array types in array assignement");
    exit(1);
  }
  
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  decrementReferenceCounter(context);
  mType->incrementReferenceCount(context, assignToValue);
  IRWriter::newStoreInst(context, assignToValue, mValueStore);
  
  return assignToValue;
}

void LocalArrayReferenceVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mType->decrementReferenceCount(context, value);
}
