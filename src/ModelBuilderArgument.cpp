//
//  ModelBuilderArgument.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IExpression.hpp"
#include "wisey/Log.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelBuilderArgument.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

bool ModelBuilderArgument::checkArgument(const Model* model) {
  if (mFieldSpecifier.substr(0, 4).compare("with")) {
    Log::e("Model builder argument should start with 'with'. e.g. .withField(value).");
    return false;
  }
  
  string fieldName = deriveFieldName();
  if (model->findField(fieldName) == NULL) {
    Log::e("Model builder could not find field '" + fieldName + "' in MODEL '" +
           model->getName() + "'");
    return false;
  }
  
  return true;
}

string ModelBuilderArgument::deriveFieldName() const {
  return "m" + mFieldSpecifier.substr(4);
}

Value* ModelBuilderArgument::getValue(IRGenerationContext& context) const {
  return mFieldExpression.generateIR(context);
}

IType* ModelBuilderArgument::getType(IRGenerationContext& context) const {
  return mFieldExpression.getType(context);
}
