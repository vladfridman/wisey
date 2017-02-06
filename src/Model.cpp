//
//  Model.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Model.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Model::~Model() {
  for (map<string, ModelField*>::iterator iterator = mFields->begin();
       iterator != mFields->end();
       iterator++) {
    ModelField* modelField = iterator->second;
    delete modelField;
  }
  delete mFields;
}

ModelField* Model::findField(string fieldName) {
  if (mFields->count(fieldName)) {
    return mFields->at(fieldName);
  }
  
  return NULL;
}

vector<string> Model::getMissingFields(set<string> givenFields) {
  vector<string> missingFields;
  
  for (map<string, ModelField*>::iterator iterator = mFields->begin();
       iterator != mFields->end();
       iterator++) {
    string modelFieldName = iterator->first;
    if (givenFields.find(modelFieldName) == givenFields.end()) {
      missingFields.push_back(modelFieldName);
    }
  }
  
  return missingFields;
}

string Model::getName() const {
  return mName;
}

llvm::Type* Model::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType->getPointerTo();
}

TypeKind Model::getTypeKind() const {
  return MODEL_TYPE;
}
