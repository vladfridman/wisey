//
//  FieldPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "AutoCast.hpp"
#include "FieldPrimitiveVariable.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldPrimitiveVariable::FieldPrimitiveVariable(string name,
                                               const IConcreteObjectType* object,
                                               int line) :
mName(name), mObject(object), mLine(line) { }

FieldPrimitiveVariable::~FieldPrimitiveVariable() {}

string FieldPrimitiveVariable::getName() const {
  return mName;
}

const IType* FieldPrimitiveVariable::getType() const {
  return mObject->findField(mName)->getType();
}

bool FieldPrimitiveVariable::isField() const {
  return true;
}

bool FieldPrimitiveVariable::isStatic() const {
  return false;
}

int FieldPrimitiveVariable::getLine() const {
  return mLine;
}

Value* FieldPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newLoadInst(context, fieldPointer, mName);
}

Value* FieldPrimitiveVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                             int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    vector<const IExpression*> arrayIndices,
                                                    int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);

  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    context.reportError(line, "Can not assign to field '" + mName + "' of object '"
                        + mObject->getTypeName() + "' because of incompatable types");
    throw 1;
  }
  Value* expressionValue = assignToExpression->generateIR(context, fieldType);
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

