//
//  IFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IFieldVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace wisey;

IField* IFieldVariable::checkAndFindFieldForAssignment(IRGenerationContext& context,
                                                       const IConcreteObjectType* object,
                                                       std::string fieldName) {
  IField* field = checkAndFindField(context, object, fieldName);
  
  if (field->isAssignable()) {
    return field;
  }
  
  Log::e("Can not assign to field " + field->getName());
  exit(1);
}

IField* IFieldVariable::checkAndFindField(IRGenerationContext& context,
                                          const IConcreteObjectType* object,
                                          std::string fieldName) {
  IField* field = object->findField(fieldName);
  
  if (field != NULL) {
    return field;
  }
  
  Log::e("Field " + fieldName + " is not found in object " + object->getTypeName());
  exit(1);
}

GetElementPtrInst* IFieldVariable::getFieldPointer(IRGenerationContext& context,
                                                   const IConcreteObjectType* object,
                                                   std::string fieldName) {
  IVariable* thisVariable = context.getThis();
  LLVMContext& llvmContext = context.getLLVMContext();
  
  IField* field = checkAndFindField(context, object, fieldName);
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
  
  Value* thisObject = thisVariable->generateIdentifierIR(context);
  
  return IRWriter::createGetElementPtrInst(context, thisObject, index);
}

