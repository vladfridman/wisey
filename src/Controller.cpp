//
//  Controller.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Controller.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Controller::~Controller() {
  mFields.clear();
  mMethods.clear();
}

Controller::Controller(string name,
             StructType* structType,
             map<string, Field*> fields,
             vector<Method*> methods,
             vector<Interface*> interfaces) {
  mName = name;
  mStructType = structType;
  mFields = fields;
  mMethods = methods;
  mInterfaces = interfaces;
  
  for (Method* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

vector<Interface*> Controller::getInterfaces() const {
  return mInterfaces;
}

Field* Controller::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

Method* Controller::findMethod(std::string methodName) const {
  if (!mNameToMethodMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodMap.at(methodName);
}

string Controller::getObjectNameGlobalVariableName() const {
  return "controller." + getName() + ".name";
}

string Controller::getName() const {
  return mName;
}

llvm::Type* Controller::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType->getPointerTo();
}

TypeKind Controller::getTypeKind() const {
  return CONTROLLER_TYPE;
}

bool Controller::canCastTo(IType* toType) const {
  // TODO implement casting for controllers
  return false;
}

bool Controller::canAutoCastTo(IType* toType) const {
  // TODO implement casting for controllers
  return false;
}

Value* Controller::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  // TODO implement casting for controllers
  return NULL;
}
