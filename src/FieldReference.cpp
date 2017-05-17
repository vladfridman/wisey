//
//  FieldReference.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/FieldReference.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

Value* FieldReference::generateIR(IRGenerationContext& context) const {
  checkAndFindField(context);
  
  Log::e("All model fields are private. Implement getters and setters to use fields");
  exit(1);
}

IType* FieldReference::getType(IRGenerationContext& context) const {
  return checkAndFindField(context)->getType();
}

Field* FieldReference::checkAndFindField(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    Log::e("Attempt to reference field '" + mFieldName + "' in a primitive type expression");
    exit(1);
  }

  Model* model = dynamic_cast<Model*>(expressionType);
  Field* field = model->findField(mFieldName);

  if (field != NULL) {
    return field;
  }
  
  Log::e("Field '" + mFieldName + "' is not found in model '" + model->getName() + "'");
  exit(1);
}

void FieldReference::releaseOwnership(IRGenerationContext &context) const {
  Log::e("Model fields can not be references and objects they reference can not be released");
  exit(1);
}
