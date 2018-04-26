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
#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalArrayOwnerVariable::LocalArrayOwnerVariable(string name,
                                                 const ArrayOwnerType* arrayOwnerType,
                                                 llvm::Value* valueStore) :
mName(name), mArrayOwnerType(arrayOwnerType), mValueStore(valueStore) {
}

LocalArrayOwnerVariable::~LocalArrayOwnerVariable() {
}

bool LocalArrayOwnerVariable::isField() const {
  return false;
}

string LocalArrayOwnerVariable::getName() const {
  return mName;
}

const wisey::ArrayOwnerType* LocalArrayOwnerVariable::getType() const {
  return mArrayOwnerType;
}

llvm::Value* LocalArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

llvm::Value* LocalArrayOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                                    context) const {
  return mValueStore;
}

bool LocalArrayOwnerVariable::isSystem() const {
  return false;
}

llvm::Value* LocalArrayOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                           IExpression* assignToExpression,
                                                           vector<const IExpression*> arrayIndices,
                                                           int line) {
  Composer::setLineNumber(context, line);
  
  Value* result = arrayIndices.size()
  ? generateArrayElementAssignment(context, assignToExpression, arrayIndices, line)
  : generateWholeArrayAssignment(context, assignToExpression, line);
  
  return result;
}

Value* LocalArrayOwnerVariable::generateArrayElementAssignment(IRGenerationContext& context,
                                                               IExpression* assignToExpression,
                                                               vector<const IExpression*>
                                                               arrayIndices,
                                                               int line) {
  Value* arrayPointer = IRWriter::newLoadInst(context, mValueStore, "");
  const ArrayType* arrayType = mArrayOwnerType->getArrayType(context);
  const IType* elementType = arrayType->getElementType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  arrayType,
                                                                  arrayPointer,
                                                                  arrayIndices);
  
  return ArrayElementAssignment::generateElementAssignment(context,
                                                           elementType,
                                                           assignToExpression,
                                                           elementStore,
                                                           line);
}

llvm::Value* LocalArrayOwnerVariable::generateWholeArrayAssignment(IRGenerationContext& context,
                                                                   IExpression* assignToExpression,
                                                                   int line) {
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, mArrayOwnerType);
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, mArrayOwnerType, line);
  
  free(context, line);
  
  IRWriter::newStoreInst(context, cast, mValueStore);

  return assignToValue;
}

void LocalArrayOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  Value* null = ConstantPointerNull::get(mArrayOwnerType->getLLVMType(context));
  IRWriter::newStoreInst(context, null, mValueStore);
}

void LocalArrayOwnerVariable::free(IRGenerationContext& context, int line) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mArrayOwnerType->free(context, value, line);
}
