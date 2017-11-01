//
//  ParameterOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ParameterOwnerVariable::ParameterOwnerVariable(string name,
                                               const IObjectOwnerType* type,
                                               Value* value) :
mName(name), mType(type), mValue(value) { }

ParameterOwnerVariable::~ParameterOwnerVariable() {
}

string ParameterOwnerVariable::getName() const {
  return mName;
}

const IType* ParameterOwnerVariable::getType() const {
  return mType;
}

Value* ParameterOwnerVariable::getValue() const {
  return mValue;
}

Value* ParameterOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                                    string llvmVariableName) const {
  return mValue;
}

Value* ParameterOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression) {
  Log::e("Assignment to method parameters is not allowed");
  exit(1);
}

void ParameterOwnerVariable::setToNull(IRGenerationContext& context) {
  PointerType* type = (PointerType*) getType()->getLLVMType(context.getLLVMContext());
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValue);
}

void ParameterOwnerVariable::free(IRGenerationContext& context) const {
  mType->free(context, mValue);
}

bool ParameterOwnerVariable::existsInOuterScope() const {
  return true;
}
