//
//  ParameterPointerOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterPointerOwnerVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterPointerOwnerVariable::ParameterPointerOwnerVariable(string name,
                                                             const LLVMPointerOwnerType* type,
                                                             Value* valueStore) :
mName(name), mType(type), mValueStore(valueStore) {
}

ParameterPointerOwnerVariable::~ParameterPointerOwnerVariable() {
}

string ParameterPointerOwnerVariable::getName() const {
  return mName;
}

const LLVMPointerOwnerType* ParameterPointerOwnerVariable::getType() const {
  return mType;
}

bool ParameterPointerOwnerVariable::isField() const {
  return false;
}

bool ParameterPointerOwnerVariable::isSystem() const {
  return false;
}

Value* ParameterPointerOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  return IRWriter::newLoadInst(context, mValueStore, "");
}

Value* ParameterPointerOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext&
                                                                    context) const {
  return mValueStore;
}

Value* ParameterPointerOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                           IExpression* assignToExpression,
                                                           vector<const IExpression*> arrayIndices,
                                                           int line) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterPointerOwnerVariable::setToNull(IRGenerationContext& context) {
  llvm::PointerType* type = getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValueStore);
}

void ParameterPointerOwnerVariable::free(IRGenerationContext& context) const {
  Value* valueLoaded = IRWriter::newLoadInst(context, mValueStore, "");
  mType->free(context, valueLoaded);
}
