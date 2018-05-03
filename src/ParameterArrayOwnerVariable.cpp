//
//  ParameterArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterArrayOwnerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterArrayOwnerVariable::ParameterArrayOwnerVariable(string name,
                                                         const ArrayOwnerType* arrayOwnerType,
                                                         Value* valueStore) :
mName(name), mArrayOwnerType(arrayOwnerType), mValueStore(valueStore) {
  assert(valueStore->getType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->isPointerTy());
  assert(valueStore->getType()->getPointerElementType()->getPointerElementType()->isStructTy());
}

ParameterArrayOwnerVariable::~ParameterArrayOwnerVariable() {
}

string ParameterArrayOwnerVariable::getName() const {
  return mName;
}

const ArrayOwnerType* ParameterArrayOwnerVariable::getType() const {
  return mArrayOwnerType;
}

bool ParameterArrayOwnerVariable::isField() const {
  return false;
}

bool ParameterArrayOwnerVariable::isSystem() const {
  return false;
}

Value* ParameterArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                                         int line) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* ParameterArrayOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                  int line) const {
  return mValueStore;
}

Value* ParameterArrayOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                         IExpression* assignToExpression,
                                                         vector<const IExpression*> arrayIndices,
                                                         int line) {
  context.reportError(line, "Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterArrayOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  Value* null = ConstantPointerNull::get(mArrayOwnerType->getLLVMType(context));
  IRWriter::newStoreInst(context, null, mValueStore);
}

void ParameterArrayOwnerVariable::free(IRGenerationContext& context, int line) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mArrayOwnerType->free(context, valueLoaded, line);
}
