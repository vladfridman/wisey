//
//  LocalReferenceArrayVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceArrayVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalReferenceArrayVariable::LocalReferenceArrayVariable(string name,
                                                         const ArrayType* type,
                                                         llvm::Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

LocalReferenceArrayVariable::~LocalReferenceArrayVariable() {
}

string LocalReferenceArrayVariable::getName() const {
  return mName;
}

const wisey::ArrayType* LocalReferenceArrayVariable::getType() const {
  return mType;
}

llvm::Value* LocalReferenceArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValueStore;
}

llvm::Value* LocalReferenceArrayVariable::generateAssignmentIR(IRGenerationContext& context,
                                                               IExpression* assignToExpression,
                                                               vector<const IExpression*>
                                                               arrayIndices,
                                                               int line) {
  Composer::pushCallStack(context, line);
  
  Value* elementStore =
  ArrayElementExpression::generateElementIR(context, mType, mValueStore, arrayIndices);
  
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  const IType* assignToType = assignToExpression->getType(context);
  const IType* scalarType = mType->getScalarType();
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, scalarType, line);
 
  Value* elementLoaded = IRWriter::newLoadInst(context, elementStore, "");
  assert(IType::isReferenceType(scalarType));
  const IObjectType* objectType = (const IObjectType*) scalarType;
  objectType->decremenetReferenceCount(context, elementLoaded);
  objectType->incremenetReferenceCount(context, newValue);
  
  IRWriter::newStoreInst(context, newValue, elementStore);
  
  Composer::popCallStack(context);
  
  return newValue;
}

void LocalReferenceArrayVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  // TODO: implement this
}
