//
//  FieldLLVMVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/FieldLLVMVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldLLVMVariable::FieldLLVMVariable(string name, const IConcreteObjectType* object, int line) :
mName(name), mObject(object), mLine(line) { }

FieldLLVMVariable::~FieldLLVMVariable() {}

string FieldLLVMVariable::getName() const {
  return mName;
}

const ILLVMType* FieldLLVMVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isNative());
  assert(!type->isReference());
  assert(!type->isOwner());
  
  return (const ILLVMType*) type;
}

bool FieldLLVMVariable::isField() const {
  return true;
}

bool FieldLLVMVariable::isSystem() const {
  return false;
}

int FieldLLVMVariable::getLine() const {
  return mLine;
}

Value* FieldLLVMVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldLLVMVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                        int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldLLVMVariable::generateAssignmentIR(IRGenerationContext& context,
                                               IExpression* assignToExpression,
                                               vector<const IExpression*> arrayIndices,
                                               int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);
  
  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    context.reportError(line, "Can not assign to field '" + mName +
                        "' of object '" + mObject->getTypeName() +
                        "' because of incompatable types");
    throw 1;
  }
  Value* expressionValue = assignToExpression->generateIR(context, fieldType);
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}
