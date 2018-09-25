//
//  IFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "IFieldVariable.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"

using namespace llvm;
using namespace wisey;

IField* IFieldVariable::checkAndFindFieldForAssignment(IRGenerationContext& context,
                                                       const IConcreteObjectType* object,
                                                       std::string fieldName,
                                                       int line) {
  IField* field = checkAndFindField(context, object, fieldName, line);
  
  if (field->isAssignable(object)) {
    return field;
  }
  
  context.reportError(line, "Can not assign to " + field->getFieldKind() +
                      " field " + field->getName());
  throw 1;
}

IField* IFieldVariable::checkAndFindField(IRGenerationContext& context,
                                          const IConcreteObjectType* object,
                                          std::string fieldName,
                                          int line) {
  IField* field = object->findField(fieldName);
  
  if (field != NULL) {
    return field;
  }
  
  context.reportError(line, "Field " + fieldName + " is not found in object " +
                      object->getTypeName());
  throw 1;
}

GetElementPtrInst* IFieldVariable::getFieldPointer(IRGenerationContext& context,
                                                   const IConcreteObjectType* object,
                                                   std::string fieldName,
                                                   int line) {
  IVariable* thisVariable = context.getThis();
  LLVMContext& llvmContext = context.getLLVMContext();
  
  IField* field = checkAndFindField(context, object, fieldName, line);
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
  
  Value* thisObject = thisVariable->generateIdentifierIR(context, line);
  
  return IRWriter::createGetElementPtrInst(context, thisObject, index);
}

