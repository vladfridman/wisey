//
//  LocalOwnerArrayVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerArrayVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalOwnerArrayVariable::LocalOwnerArrayVariable(string name,
                                       const ArrayType* type,
                                       llvm::Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

LocalOwnerArrayVariable::~LocalOwnerArrayVariable() {
}

string LocalOwnerArrayVariable::getName() const {
  return mName;
}

const wisey::ArrayType* LocalOwnerArrayVariable::getType() const {
  return mType;
}

llvm::Value* LocalOwnerArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValueStore;
}

llvm::Value* LocalOwnerArrayVariable::generateAssignmentIR(IRGenerationContext& context,
                                                           IExpression* assignToExpression,
                                                           vector<const IExpression*> arrayIndices,
                                                           int line) {
  Composer::pushCallStack(context, line);
  
  Value* elementStore =
    ArrayElementExpression::generateElementIR(context, mType, mValueStore, arrayIndices);

  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_RELEASE);
  const IType* assignToType = assignToExpression->getType(context);
  const IType* scalarType = mType->getScalarType();
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, scalarType, line);
  
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  assert(IType::isOwnerType(scalarType));
  ((const IObjectOwnerType*) scalarType)->free(context, elementLoaded);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  Composer::popCallStack(context);
  
  return newValue;
}

void LocalOwnerArrayVariable::free(IRGenerationContext& context) const {
  // TODO: implement this
}

void LocalOwnerArrayVariable::setToNull(IRGenerationContext& context) {
  // TODO:: implement this
}

