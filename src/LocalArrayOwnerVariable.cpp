//
//  LocalArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
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
  return mValueStore;
}

llvm::Value* LocalArrayOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                           IExpression* assignToExpression,
                                                           vector<const IExpression*> arrayIndices,
                                                           int line) {
  if (!arrayIndices.size()) {
    return generateWholeArrayAssignment(context, assignToExpression, line);
  }
  
  const IType* scalarType = mType->getArrayType()->getScalarType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  mType->getArrayType(),
                                                                  mValueStore,
                                                                  arrayIndices);
  
  if (IType::isOwnerType(scalarType)) {
    return generateOwnerElementAssignment(context, assignToExpression, elementStore, line);
  }
  
  if (IType::isReferenceType(scalarType)) {
    return generateReferenceElementAssignment(context, assignToExpression, elementStore, line);
  }
  
  assert(scalarType->getTypeKind() == PRIMITIVE_TYPE);
  return generatePrimitiveElementAssignment(context, assignToExpression, elementStore, line);
}

llvm::Value* LocalArrayOwnerVariable::generateWholeArrayAssignment(IRGenerationContext& context,
                                                                   IExpression* assignToExpression,
                                                                   int line) {
  const IType* assignToType = assignToExpression->getType(context);
  if (assignToType != mType) {
    Log::e("Incompatable array types in array assignement");
    exit(1);
  }
  
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_RELEASE);
  IRWriter::newStoreInst(context, assignToValue, mValueStore);
  
  return assignToValue;
}

llvm::Value* LocalArrayOwnerVariable::
generateOwnerElementAssignment(IRGenerationContext& context,
                               IExpression* assignToExpression,
                               llvm::Value* elementStore,
                               int line) {
  const IType* scalarType = mType->getArrayType()->getScalarType();
  
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_RELEASE);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, scalarType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, elementLoaded, int8pointer);
  ((const IObjectOwnerType*) scalarType)->free(context, bitcast);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* LocalArrayOwnerVariable::
generateReferenceElementAssignment(IRGenerationContext& context,
                                   IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  const IType* scalarType = mType->getArrayType()->getScalarType();
  
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, scalarType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  const IObjectType* objectType = (const IObjectType*) scalarType;
  objectType->decremenetReferenceCount(context, elementLoaded);
  objectType->incremenetReferenceCount(context, newValue);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

llvm::Value* LocalArrayOwnerVariable::
generatePrimitiveElementAssignment(IRGenerationContext& context,
                                   IExpression* assignToExpression,
                                   llvm::Value* elementStore,
                                   int line) {
  const IType* scalarType = mType->getArrayType()->getScalarType();
  
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, scalarType, line);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  return newValue;
}

void LocalArrayOwnerVariable::setToNull(IRGenerationContext& context) {
  Value* null = ConstantPointerNull::get(mType->getLLVMType(context)->getPointerTo());
  IRWriter::newStoreInst(context, null, mValueStore);
}

void LocalArrayOwnerVariable::free(IRGenerationContext& context) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, value);
}
