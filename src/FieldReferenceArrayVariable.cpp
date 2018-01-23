//
//  FieldReferenceArrayVariable.cpp
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
#include "wisey/FieldReferenceArrayVariable.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FieldReferenceArrayVariable::FieldReferenceArrayVariable(string name,
                                                         const IConcreteObjectType* object) :
mName(name),
mObject(object) { }

FieldReferenceArrayVariable::~FieldReferenceArrayVariable() {
}

string FieldReferenceArrayVariable::getName() const {
  return mName;
}

const IType* FieldReferenceArrayVariable::getType() const {
  return mObject->findField(mName)->getType();
}

llvm::Value* FieldReferenceArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return fieldPointer;
}

llvm::Value* FieldReferenceArrayVariable::generateAssignmentIR(IRGenerationContext& context,
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
  assert(IType::isReferenceType(elementType));
  const IObjectType* objectType = (const IObjectType*) elementType;

  const IType* assignToType = assignToExpression->getType(context);
  if (!assignToType->canAutoCastTo(elementType)) {
    Log::e("Can not assign to field '" + mName + "' of object '" + mObject->getTypeName() +
           "' because of incompatable types");
    exit(1);
  }
  
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);

  Value* previousValue = IRWriter::newLoadInst(context, element, "");
  objectType->decrementReferenceCount(context, previousValue);
  objectType->incrementReferenceCount(context, newValue);

  IRWriter::newStoreInst(context, newValue, element);
  
  Composer::popCallStack(context);

  return newValue;
}

void FieldReferenceArrayVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  /** Decremented using object destructor */
}
