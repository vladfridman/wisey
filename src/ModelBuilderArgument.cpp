//
//  ModelBuilderArgument.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/ModelBuilderArgument.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

bool ModelBuilderArgument::checkArgument(Model* model) {
  if (mFieldSpecifier.substr(0, 4).compare("with")) {
    Log::e("Model builder argument should start with 'with'. e.g. .withField(value).");
    return false;
  }
  
  string fieldName = mFieldSpecifier.substr(4);
  transform(fieldName.begin(), fieldName.begin() + 1, fieldName.begin(), ::tolower);
  if (model->findField(fieldName) == NULL) {
    Log::e("Model could not find field '" + fieldName + "' in MODEL '" +
           model->getStructType()->getName().str() + "'");
    return false;
  }
  
  return true;
}
