//
//  LocalPrimitiveArrayVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveArrayVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalPrimitiveArrayVariable::LocalPrimitiveArrayVariable(string name,
                                       const ArrayType* type,
                                       llvm::Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

LocalPrimitiveArrayVariable::~LocalPrimitiveArrayVariable() {
}

string LocalPrimitiveArrayVariable::getName() const {
  return mName;
}

const wisey::ArrayType* LocalPrimitiveArrayVariable::getType() const {
  return mType;
}

llvm::Value* LocalPrimitiveArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValueStore;
}

llvm::Value* LocalPrimitiveArrayVariable::generateAssignmentIR(IRGenerationContext& context,
                                                               IExpression* assignToExpression,
                                                               vector<const IExpression*>
                                                               arrayIndices,
                                                               int line) {
  Value* element = ArrayElementExpression::generateElementIR(context,
                                                             mType,
                                                             mValueStore,
                                                             arrayIndices);
  Value* assignToValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  const IType* assignToType = assignToExpression->getType(context);
  const IType* scalarType = mType->getScalarType();
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, scalarType, line);
  IRWriter::newStoreInst(context, newValue, element);
  
  return newValue;
}

