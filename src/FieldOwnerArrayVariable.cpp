//
//  FieldOwnerArrayVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/Composer.hpp"
#include "wisey/DecrementReferencesInArrayFunction.hpp"
#include "wisey/FieldOwnerArrayVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FieldOwnerArrayVariable::FieldOwnerArrayVariable(string name,
                                                         const IConcreteObjectType* object) :
mName(name),
mObject(object) { }

FieldOwnerArrayVariable::~FieldOwnerArrayVariable() {
}

string FieldOwnerArrayVariable::getName() const {
  return mName;
}

const IType* FieldOwnerArrayVariable::getType() const {
  return mObject->findField(mName)->getType();
}

llvm::Value* FieldOwnerArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return fieldPointer;
}

llvm::Value* FieldOwnerArrayVariable::generateAssignmentIR(IRGenerationContext& context,
                                                               IExpression* assignToExpression,
                                                               vector<const IExpression*>
                                                               arrayIndices,
                                                               int line) {
  Composer::pushCallStack(context, line);
  
  Field* field = checkAndFindFieldForAssignment(context, mObject, mName);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  Value* element = ArrayElementExpression::generateElementIR(context,
                                                             (const ArrayType*) field->getType(),
                                                             fieldPointer,
                                                             arrayIndices);
  const IType* elementType = ((const wisey::ArrayType*) field->getType())->getScalarType();
  assert(IType::isOwnerType(elementType));
  const IObjectOwnerType* objectOwnerType = (const IObjectOwnerType*) elementType;
  
  const IType* assignToType = assignToExpression->getType(context);
  if (!assignToType->canAutoCastTo(elementType)) {
    Log::e("Can not assign to field '" + mName + "' of object '" + mObject->getTypeName() +
           "' because of incompatable types");
    exit(1);
  }
  
  Value* assignToValue = assignToExpression->generateIR(context, elementType);
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  
  Value* previousValue = IRWriter::newLoadInst(context, element, "");
  Type* int8pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, previousValue, int8pointer);
  objectOwnerType->free(context, bitcast);
  
  IRWriter::newStoreInst(context, newValue, element);
  
  Composer::popCallStack(context);
  
  return newValue;
}

void FieldOwnerArrayVariable::free(IRGenerationContext& context) const {
  /** Freed using object destructor */
}

void FieldOwnerArrayVariable::setToNull(IRGenerationContext& context) {
  // Not used, the element is set to null after ownership transfer in {@link ArrayElementExpression}
}

