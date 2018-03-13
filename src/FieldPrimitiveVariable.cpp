//
//  FieldPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldPrimitiveVariable::FieldPrimitiveVariable(string name, const IConcreteObjectType* object) :
mName(name),
mObject(object) { }

FieldPrimitiveVariable::~FieldPrimitiveVariable() {}

string FieldPrimitiveVariable::getName() const {
  return mName;
}

const IType* FieldPrimitiveVariable::getType() const {
  return mObject->findField(mName)->getType();
}

bool FieldPrimitiveVariable::isSystem() const {
  return false;
}

Value* FieldPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    vector<const IExpression*> arrayIndices,
                                                    int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName);

  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    Log::e("Can not assign to field '" + mName + "' of object '" + mObject->getTypeName() +
           "' because of incompatable types");
    exit(1);
  }
  Value* expressionValue = assignToExpression->generateIR(context, fieldType);
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

