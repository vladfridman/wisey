//
//  ModelFieldReference.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/Log.hpp"
#include "yazyk/ModelFieldReference.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* ModelFieldReference::generateIR(IRGenerationContext& context) const {
  checkAndFindField(context);
  
  Log::e("All model fields are private. Implement getters and setters to use fields");
  exit(1);
}

IType* ModelFieldReference::getType(IRGenerationContext& context) const {
  return checkAndFindField(context)->getType();
}

ModelField* ModelFieldReference::checkAndFindField(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Attempt to reference field '" + mFieldName + "' in a primitive type expression");
    exit(1);
  }

  Model* model = (Model*) expressionType;
  ModelField* modelField = model->findField(mFieldName);

  if (modelField != NULL) {
    return modelField;
  }
  
  Log::e("Field '" + mFieldName + "' is not found in model '" + model->getName() + "'");
  exit(1);
}
