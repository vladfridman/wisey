//
//  FieldPointerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/FieldPointerVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldPointerVariable::FieldPointerVariable(string name, const IConcreteObjectType* object) :
mName(name),
mObject(object) { }

FieldPointerVariable::~FieldPointerVariable() {}

string FieldPointerVariable::getName() const {
  return mName;
}

const LLVMPointerType* FieldPointerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isNative());
  assert(type->isPointer());

  return (const LLVMPointerType*) type;
}

bool FieldPointerVariable::isField() const {
  return true;
}

bool FieldPointerVariable::isSystem() const {
  return false;
}

Value* FieldPointerVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newLoadInst(context, fieldPointer, mName);
}

Value* FieldPointerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                           int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldPointerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    vector<const IExpression*> arrayIndices,
                                                    int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName);
  
  const IType* expressionType = assignToExpression->getType(context);
  assert(field->getType()->isPointer());
  const IReferenceType* fieldType = (const IReferenceType*) field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    context.reportError(line, "Can not assign to field '" + mName + "' of object '" +
                        mObject->getTypeName() + "' because of incompatable types");
    exit(1);
  }
  Value* expressionValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}
