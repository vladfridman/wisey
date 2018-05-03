//
//  LocalLLVMVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalLLVMVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LocalLLVMVariable::LocalLLVMVariable(string name,
                                     const ILLVMType* type,
                                     Value* valueStore,
                                     int line) :
mName(name), mType(type), mValueStore(valueStore), mLine(line) { }

LocalLLVMVariable::~LocalLLVMVariable() {}

string LocalLLVMVariable::getName() const {
  return mName;
}

const ILLVMType* LocalLLVMVariable::getType() const {
  return mType;
}

bool LocalLLVMVariable::isField() const {
  return false;
}

bool LocalLLVMVariable::isSystem() const {
  return false;
}

int LocalLLVMVariable::getLine() const {
  return mLine;
}

Value* LocalLLVMVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* LocalLLVMVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                        int line) const {
  return mValueStore;
}

Value* LocalLLVMVariable::generateAssignmentIR(IRGenerationContext& context,
                                               IExpression* assignToExpression,
                                               std::vector<const IExpression*> arrayIndices,
                                               int line) {
  Value* assignToValue = assignToExpression->generateIR(context, mType);
  const IType* assignToType = assignToExpression->getType(context);
  Value* castAssignToValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType, line);
  IRWriter::newStoreInst(context, castAssignToValue, mValueStore);
  
  return castAssignToValue;
}

