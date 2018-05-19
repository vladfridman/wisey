//
//  LocalImmutableArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"
#include "wisey/LocalImmutableArrayOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalImmutableArrayOwnerVariable::
LocalImmutableArrayOwnerVariable(string name,
                                 const ImmutableArrayOwnerType* immutableArrayOwnerType,
                                 llvm::Value* valueStore,
                                 int line) :
mName(name),
mImmutableArrayOwnerType(immutableArrayOwnerType),
mValueStore(valueStore),
mLine(line) {
}

LocalImmutableArrayOwnerVariable::~LocalImmutableArrayOwnerVariable() {
}

bool LocalImmutableArrayOwnerVariable::isField() const {
  return false;
}

string LocalImmutableArrayOwnerVariable::getName() const {
  return mName;
}

const ImmutableArrayOwnerType* LocalImmutableArrayOwnerVariable::getType() const {
  return mImmutableArrayOwnerType;
}

llvm::Value* LocalImmutableArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                                                    int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

llvm::Value* LocalImmutableArrayOwnerVariable::
generateIdentifierReferenceIR(IRGenerationContext& context, int line) const {
  return mValueStore;
}

bool LocalImmutableArrayOwnerVariable::isSystem() const {
  return false;
}

int LocalImmutableArrayOwnerVariable::getLine() const {
  return mLine;
}

llvm::Value* LocalImmutableArrayOwnerVariable::
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

Value* LocalImmutableArrayOwnerVariable::
generateArrayElementAssignment(IRGenerationContext& context,
                               IExpression* assignToExpression,
                               vector<const IExpression*> arrayIndices,
                               int line) {
  context.reportError(line, "Attempting to change a value in an immutable array");
  throw 1;
}

llvm::Value* LocalImmutableArrayOwnerVariable::
generateWholeArrayAssignment(IRGenerationContext& context,
                             IExpression* assignToExpression,
                             int line) {
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, mImmutableArrayOwnerType);
  Value* cast = AutoCast::maybeCast(context,
                                    assignToType,
                                    assignToValue,
                                    mImmutableArrayOwnerType,
                                    line);
  
  free(context, NULL, line);
  
  IRWriter::newStoreInst(context, cast, mValueStore);
  
  return assignToValue;
}

void LocalImmutableArrayOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  Value* null = ConstantPointerNull::get(mImmutableArrayOwnerType->getLLVMType(context));
  IRWriter::newStoreInst(context, null, mValueStore);
}

void LocalImmutableArrayOwnerVariable::free(IRGenerationContext& context,
                                            Value* exception,
                                            int line) const {
  Value* value = IRWriter::newLoadInst(context, mValueStore, "");
  mImmutableArrayOwnerType->free(context, value, exception, line);
}
