//
//  FieldOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldOwnerVariable::FieldOwnerVariable(string name,
                                       const IConcreteObjectType* object) :
mName(name), mObject(object) { }

FieldOwnerVariable::~FieldOwnerVariable() {}

string FieldOwnerVariable::getName() const {
  return mName;
}

const IObjectOwnerType* FieldOwnerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(IType::isOwnerType(type));
  
  return (const IObjectOwnerType*) type;
}

Value* FieldOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                vector<const IExpression*> arrayIndices,
                                                int line) {
  Field* field = checkAndFindFieldForAssignment(context, mObject, mName);

  Composer::pushCallStack(context, line);
  
  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(fieldType)) {
    Log::e("Can not assign to field '" + mName + "' of object '" + mObject->getTypeName() +
           "' because of incompatable types");
    exit(1);
  }
  Value* expressionValue = assignToExpression->generateIR(context, IR_GENERATION_RELEASE);
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");
  
  ((IObjectOwnerType*) field->getType())->free(context, fieldPointerLoaded);
  
  Value* value = IRWriter::newStoreInst(context, cast, fieldPointer);

  Composer::popCallStack(context);
  
  return value;
}

void FieldOwnerVariable::setToNull(IRGenerationContext& context) {
  PointerType* type = (PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void FieldOwnerVariable::free(IRGenerationContext& context) const {
  /** Freed using object destructor */
}

