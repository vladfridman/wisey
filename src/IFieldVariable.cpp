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

Field* IFieldVariable::checkAndFindFieldForAssignment(IRGenerationContext& context,
                                                      const IConcreteObjectType* object,
                                                      std::string fieldName) {
  Field* field = checkAndFindField(context, object, fieldName);
  
  if (field->isAssignable()) {
    return field;
  }
  
  Log::e("Can not assign to field " + field->getName());
  exit(1);
}

Field* IFieldVariable::checkAndFindField(IRGenerationContext& context,
                                         const IConcreteObjectType* object,
                                         std::string fieldName) {
  Field* field = object->findField(fieldName);
  
  if (field != NULL) {
    return field;
  }
  
  Log::e("Field " + fieldName + " is not found in object " + object->getName());
  exit(1);
}

GetElementPtrInst* IFieldVariable::getFieldPointer(IRGenerationContext& context,
                                                   const IConcreteObjectType* object,
                                                   std::string fieldName) {
  IVariable* thisVariable = context.getThis();
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Field* field = checkAndFindField(context, object, fieldName);
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
  Value* thisLoaded = IRWriter::newLoadInst(context, thisVariable->getValue(), "");
  
  return IRWriter::createGetElementPtrInst(context, thisLoaded, index);
}

