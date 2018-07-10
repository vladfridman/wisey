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
#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayReferenceVariable.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalArrayReferenceVariable::LocalArrayReferenceVariable(string name,
                                                         const ArrayType* arrayType,
                                                         llvm::Value* valueStore,
                                                         int line) :
mName(name), mArrayType(arrayType), mValueStore(valueStore), mLine(line) {
}

LocalArrayReferenceVariable::~LocalArrayReferenceVariable() {
}

string LocalArrayReferenceVariable::getName() const {
  return mName;
}

const wisey::ArrayType* LocalArrayReferenceVariable::getType() const {
  return mArrayType;
}

bool LocalArrayReferenceVariable::isField() const {
  return false;
}

bool LocalArrayReferenceVariable::isStatic() const {
  return false;
}

int LocalArrayReferenceVariable::getLine() const {
  return mLine;
}

llvm::Value* LocalArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                               int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

llvm::Value* LocalArrayReferenceVariable::
generateIdentifierReferenceIR(IRGenerationContext& context, int line) const {
  return mValueStore;
}

llvm::Value* LocalArrayReferenceVariable::
generateAssignmentIR(IRGenerationContext& context,
                     IExpression* assignToExpression,
                     vector<const IExpression*> arrayIndices,
                     int line) {
  Composer::setLineNumber(context, line);
  
  Value* result = arrayIndices.size()
  ? generateArrayElementAssignment(context, assignToExpression, arrayIndices, line)
  : generateWholeArrayAssignment(context, assignToExpression, line);
  
  return result;
}

Value* LocalArrayReferenceVariable::generateArrayElementAssignment(IRGenerationContext& context,
                                                                   IExpression* assignToExpression,
                                                                   vector<const IExpression*>
                                                                   arrayIndices,
                                                                   int line) {
  Value* arrayPointer = IRWriter::newLoadInst(context, mValueStore, "");
  const IType* elementType = mArrayType->getElementType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  mArrayType,
                                                                  arrayPointer,
                                                                  arrayIndices,
                                                                  line);
  
  return ArrayElementAssignment::generateElementAssignment(context,
                                                           elementType,
                                                           assignToExpression,
                                                           elementStore,
                                                           line);
}

llvm::Value* LocalArrayReferenceVariable::
generateWholeArrayAssignment(IRGenerationContext& context,
                             IExpression* assignToExpression,
                             int line) {

  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, mArrayType);
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, mArrayType, line);
  decrementReferenceCounter(context);
  mArrayType->incrementReferenceCount(context, cast);
  IRWriter::newStoreInst(context, cast, mValueStore);
  
  return assignToValue;
}

void LocalArrayReferenceVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mArrayType->decrementReferenceCount(context, value);
}
