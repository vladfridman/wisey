//
//  LocalPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalPrimitiveVariable::LocalPrimitiveVariable(string name,
                                               const IPrimitiveType* type,
                                               Value* valueStore,
                                               int line) :
mName(name), mType(type), mValueStore(valueStore), mLine(line) { }

LocalPrimitiveVariable::~LocalPrimitiveVariable() {}

string LocalPrimitiveVariable::getName() const {
  return mName;
}

const IPrimitiveType* LocalPrimitiveVariable::getType() const {
  return mType;
}

bool LocalPrimitiveVariable::isField() const {
  return false;
}

bool LocalPrimitiveVariable::isSystem() const {
  return false;
}

int LocalPrimitiveVariable::getLine() const {
  return mLine;
}

Value* LocalPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalPrimitiveVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                             int line) const {
  return mValueStore;
}

Value* LocalPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    std::vector<const IExpression*> arrayIndices,
                                                    int line) {
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* castAssignToValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  IRWriter::newStoreInst(context, castAssignToValue, mValueStore);
  
  return castAssignToValue;
}

