//
//  ObjectFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ObjectFieldVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string ObjectFieldVariable::getName() const {
  return mName;
}

IType* ObjectFieldVariable::getType() const {
  return mObject->findField(mName)->getType();
}

Value* ObjectFieldVariable::getValue() const {
  return mValue;
}

Value* ObjectFieldVariable::generateIdentifierIR(IRGenerationContext& context,
                                                 string llvmVariableName) const {
  IVariable* thisVariable = context.getScopes().getVariable("this");
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* loadedValue = IRWriter::newLoadInst(context, thisVariable->getValue(), "this");
  
  Field* field = checkAndFindField(context);
  Value* index[2];
  index[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
  
  GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, loadedValue, index);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* ObjectFieldVariable::generateAssignmentIR(IRGenerationContext& context,
                                                 IExpression& assignToExpression) {
  Field* field = checkAndFindField(context);
  IType* assignToType = assignToExpression.getType(context);
  IType* toType = field->getType();
  if (!assignToType->canAutoCastTo(toType)) {
    Log::e("Can not assign to field '" + mName + "' of object '" + mObject->getName() +
           "' because of incompatable types");
    exit(1);
  }
  Value* assignToValue = AutoCast::maybeCast(context,
                                             assignToType,
                                             assignToExpression.generateIR(context),
                                             toType);
  
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* index[2];
  index[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
  IVariable* thisVariable = context.getScopes().getVariable("this");
  Value* loadedValue = IRWriter::newLoadInst(context, thisVariable->getValue(), "this");
  GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, loadedValue, index);
  
  return IRWriter::newStoreInst(context, assignToValue, fieldPointer);
}

void ObjectFieldVariable::free(IRGenerationContext& context) const {
  /** Not implmeneted yet */
}

Field* ObjectFieldVariable::checkAndFindField(IRGenerationContext& context) const {
  Field* field = mObject->findField(mName);
  
  if (field != NULL) {
    return field;
  }
  
  Log::e("Field '" + mName + "' is not found in object '" + mObject->getName() + "'");
  exit(1);
}
