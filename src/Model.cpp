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

StructType* Model::getStructType() {
  return mStructType;
}

ModelField* Model::findField(string fieldName) {
  if (mFields->count(fieldName)) {
    return mFields->at(fieldName);
  }
  
  return NULL;
}

map<string, ModelField*>* Model::getFields() {
  return mFields;
}

Model::~Model() {
  for (map<string, ModelField*>::iterator iterator = mFields->begin();
       iterator != mFields->end();
       iterator++) {
    ModelField* modelField = iterator->second;
    delete modelField;
  }
  delete mFields;
}
