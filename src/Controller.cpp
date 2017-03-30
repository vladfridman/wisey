//
//  Controller.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Controller.hpp"
#include "yazyk/Field.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Controller::~Controller() {
  mReceivedFields.clear();
  mInjectedFields.clear();
  mStateFields.clear();
  mMethods.clear();
}

Controller::Controller(string name,
                       StructType* structType,
                       vector<Field*> receivedFields,
                       vector<Field*> injectedFields,
                       vector<Field*> stateFields,
                       vector<Method*> methods,
                       vector<Interface*> interfaces) {
  mName = name;
  mStructType = structType;
  mReceivedFields = receivedFields;
  mInjectedFields = injectedFields;
  mStateFields = stateFields;
  mMethods = methods;
  mInterfaces = interfaces;
  
  for (Field* field : receivedFields) {
    mFields[field->getName()] = field;
  }
  for (Field* field : injectedFields) {
    mFields[field->getName()] = field;
  }
  for (Field* field : stateFields) {
    mFields[field->getName()] = field;
  }
  for (Method* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
  mFlattenedInterfaceHierarchy = createFlattenedInterfaceHierarchy();
}

vector<Interface*> Controller::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Controller::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

string Controller::getTypeTableName() const {
  return "controller." + getName() + ".typetable";
}

string Controller::getVTableName() const {
  return "controller." + getName() + ".vtable";
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

vector<Interface*> Controller::createFlattenedInterfaceHierarchy() const {
  vector<Interface*> result;
  for (Interface* interface : mInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
  return result;
}

void Controller::addInterfaceAndItsParents(vector<Interface*>& result, Interface* interface) const {
  result.push_back(interface);
  vector<Interface*> parentInterfaces = interface->getParentInterfaces();
  for (Interface* interface : parentInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
}
