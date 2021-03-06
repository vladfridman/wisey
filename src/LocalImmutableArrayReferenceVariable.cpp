//
//  LocalImmutableArrayReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "AutoCast.hpp"
#include "ArrayElementAssignment.hpp"
#include "ArrayElementExpression.hpp"
#include "ArrayOwnerType.hpp"
#include "Composer.hpp"
#include "IRWriter.hpp"
#include "LocalImmutableArrayReferenceVariable.hpp"
#include "Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalImmutableArrayReferenceVariable::
LocalImmutableArrayReferenceVariable(string name,
                                     const ImmutableArrayType* immutableArrayType,
                                     llvm::Value* valueStore,
                                     int line) :
mName(name), mImmutableArrayType(immutableArrayType), mValueStore(valueStore), mLine(line) {
}

LocalImmutableArrayReferenceVariable::~LocalImmutableArrayReferenceVariable() {
}

string LocalImmutableArrayReferenceVariable::getName() const {
  return mName;
}

const ImmutableArrayType* LocalImmutableArrayReferenceVariable::getType() const {
  return mImmutableArrayType;
}

bool LocalImmutableArrayReferenceVariable::isField() const {
  return false;
}

bool LocalImmutableArrayReferenceVariable::isStatic() const {
  return false;
}

int LocalImmutableArrayReferenceVariable::getLine() const {
  return mLine;
}

llvm::Value* LocalImmutableArrayReferenceVariable::generateIdentifierIR(IRGenerationContext&
                                                                        context,
                                                                        int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

llvm::Value* LocalImmutableArrayReferenceVariable::
generateIdentifierReferenceIR(IRGenerationContext& context, int line) const {
  return mValueStore;
}

llvm::Value* LocalImmutableArrayReferenceVariable::
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

Value* LocalImmutableArrayReferenceVariable::
generateArrayElementAssignment(IRGenerationContext& context,
                               IExpression* assignToExpression,
                               vector<const IExpression*> arrayIndices,
                               int line) {
  context.reportError(line, "Attempting to change a value in an immutable array");
  throw 1;
}

llvm::Value* LocalImmutableArrayReferenceVariable::
generateWholeArrayAssignment(IRGenerationContext& context,
                             IExpression* assignToExpression,
                             int line) {
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, mImmutableArrayType);
  Value* cast = AutoCast::maybeCast(context,
                                    assignToType,
                                    assignToValue,
                                    mImmutableArrayType,
                                    line);
  decrementReferenceCounter(context);
  mImmutableArrayType->incrementReferenceCount(context, cast);
  IRWriter::newStoreInst(context, cast, mValueStore);
  
  return assignToValue;
}

void LocalImmutableArrayReferenceVariable::
decrementReferenceCounter(IRGenerationContext& context) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mImmutableArrayType->decrementReferenceCount(context, value);
}

