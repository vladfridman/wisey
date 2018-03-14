//
//  FieldNativeVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/FieldNativeVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldNativeVariable::FieldNativeVariable(string name, const IConcreteObjectType* object) :
mName(name),
mObject(object) { }

FieldNativeVariable::~FieldNativeVariable() {}

string FieldNativeVariable::getName() const {
  return mName;
}

const NativeType* FieldNativeVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isNative());
  
  return (const NativeType*) type;
}

bool FieldNativeVariable::isField() const {
  return true;
}

bool FieldNativeVariable::isSystem() const {
  return false;
}

Value* FieldNativeVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldNativeVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return getFieldPointer(context, mObject, mName);
}

Value* FieldNativeVariable::generateAssignmentIR(IRGenerationContext& context,
                                                 IExpression* assignToExpression,
                                                 vector<const IExpression*> arrayIndices,
                                                 int line) {
  assert(false);
}

