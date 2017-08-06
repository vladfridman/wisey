//
//  HeapOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string HeapOwnerVariable::getName() const {
  return mName;
}

const IObjectOwnerType* HeapOwnerVariable::getType() const {
  return mType;
}

Value* HeapOwnerVariable::getValue() const {
  return mValue;
}

Value* HeapOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                               string llvmVariableName) const {
  return IRWriter::newLoadInst(context, mValue, "heapObjectIdentifier");
}

Value* HeapOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                               IExpression* assignToExpression) {
  Value* assignToValue = assignToExpression->generateIR(context);
  const IType* assignToType = assignToExpression->getType(context);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  
  Value* objectPointer = IRWriter::newLoadInst(context, mValue, "objectToFree");
  Composer::freeIfNotNull(context, objectPointer);
  IRWriter::newStoreInst(context, newValue, mValue);
  
  return mValue;
}

void HeapOwnerVariable::setToNull(IRGenerationContext& context) const {
  PointerType* type = (PointerType*) getType()->getLLVMType(context.getLLVMContext());
  Value* null = ConstantPointerNull::get(type);
  IRWriter::newStoreInst(context, null, mValue);
}

void HeapOwnerVariable::free(IRGenerationContext& context) const {
  Value* objectPointer = IRWriter::newLoadInst(context, mValue, "variableObject");
  
  Value* thisPointer = mType->getTypeKind() == INTERFACE_OWNER_TYPE
  ? Interface::getOriginalObject(context, objectPointer)
  : objectPointer;
  
  IRWriter::createFree(context, thisPointer);
}

bool HeapOwnerVariable::existsInOuterScope() const {
  return false;
}
