//
//  LocalArrayVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayElementExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalArrayVariable::LocalArrayVariable(string name,
                                       const ArrayType* type,
                                       llvm::Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

LocalArrayVariable::~LocalArrayVariable() {
}

string LocalArrayVariable::getName() const {
  return mName;
}

const wisey::ArrayType* LocalArrayVariable::getType() const {
  return mType;
}

llvm::Value* LocalArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return mValueStore;
}

llvm::Value* LocalArrayVariable::generateAssignmentIR(IRGenerationContext& context,
                                                      IExpression* assignToExpression,
                                                      vector<const IExpression*> arrayIndices,
                                                      int line) {
  Value* element = ArrayElementExpression::generateElementIR(context,
                                                             mType,
                                                             mValueStore,
                                                             arrayIndices);
  
  Value* expressionValue = assignToExpression->generateIR(context, IR_GENERATION_NORMAL);
  return IRWriter::newStoreInst(context, expressionValue, element);
}

