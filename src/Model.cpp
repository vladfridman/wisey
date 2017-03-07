//
//  Model.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Cast.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Model::~Model() {
  mFields->clear();
  mMethods->clear();

  delete mFields;
  delete mMethods;
}

ModelField* Model::findField(string fieldName) const {
  if (!mFields->count(fieldName)) {
    return NULL;
  }
  
  return mFields->at(fieldName);
}

vector<string> Model::getMissingFields(set<string> givenFields) const {
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

Method* Model::findMethod(std::string methodName) const {
  if (!mMethods->count(methodName)) {
    return NULL;
  }
  
  return mMethods->at(methodName);
}

string Model::getVTableName() const {
  return "model." + getName() + ".vtable";
}

vector<Interface*> Model::getInterfaces() const {
  return mInterfaces;
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

bool Model::canCastTo(IType* toType) const {
  if (toType == this) {
    return true;
  }
  if (toType->getTypeKind() == MODEL_TYPE) {
    return false;
  }
  if (toType->getTypeKind() == INTERFACE_TYPE && doesImplmenetInterface((Interface*) toType)) {
    return true;
  }
  return false;
}

bool Model::canCastLosslessTo(IType* toType) const {
  return canCastTo(toType);
}

Value* Model::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == this) {
    return fromValue;
  }
  if (!canCastTo(toType)) {
    Cast::exitIncopatibleTypes(this, toType);
    return NULL;
  }
  
  return new BitCastInst(fromValue,
                         toType->getLLVMType(context.getLLVMContext()),
                         "",
                         context.getBasicBlock());
}

bool Model::doesImplmenetInterface(Interface* interface) const {
  for (Interface* implementedInterface : mInterfaces) {
    if (implementedInterface == interface) {
      return true;
    }
  }
  return false;
}
