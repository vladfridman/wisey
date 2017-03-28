//
//  Model.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Cast.hpp"
#include "yazyk/Environment.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Model::~Model() {
  mFields.clear();
  mMethods.clear();
}

Model::Model(string name,
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
  mFlattenedInterfaceHierarchy = createFlattenedInterfaceHierarchy();
}

Field* Model::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

vector<string> Model::getMissingFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (map<string, Field*>::const_iterator iterator = mFields.begin();
       iterator != mFields.end();
       iterator++) {
    string fieldName = iterator->first;
    if (givenFields.find(fieldName) == givenFields.end()) {
      missingFields.push_back(fieldName);
    }
  }
  
  return missingFields;
}

Method* Model::findMethod(std::string methodName) const {
  if (!mNameToMethodMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodMap.at(methodName);
}

string Model::getVTableName() const {
  return "model." + getName() + ".vtable";
}

string Model::getObjectNameGlobalVariableName() const {
  return "model." + getName() + ".name";
}

string Model::getTypeTableName() const {
  return "model." + getName() + ".typetable";
}

vector<Interface*> Model::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Model::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

bool Model::doesImplmentInterface(Interface* interface) const {
  // TODO: optimize this
  return getInterfaceIndex(interface) >= 0;
}

vector<Interface*> Model::createFlattenedInterfaceHierarchy() const {
  vector<Interface*> result;
  for (Interface* interface : mInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
  return result;
}

void Model::addInterfaceAndItsParents(vector<Interface*>& result, Interface* interface) const {
  result.push_back(interface);
  vector<Interface*> parentInterfaces = interface->getParentInterfaces();
  for (Interface* interface : parentInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
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
  if (toType->getTypeKind() == INTERFACE_TYPE &&
      getInterfaceIndex(dynamic_cast<Interface*>(toType)) >= 0) {
    return true;
  }
  return false;
}

bool Model::canAutoCastTo(IType* toType) const {
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
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* basicBlock = context.getBasicBlock();
  Interface* interface = dynamic_cast<Interface*>(toType);
  int interfaceIndex = getInterfaceIndex(interface);
  if (interfaceIndex == 0) {
    return new BitCastInst(fromValue, interface->getLLVMType(llvmContext), "", basicBlock);
  }
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* bitcast = new BitCastInst(fromValue, int8Type->getPointerTo(), "", basicBlock);
  Value *Idx[1];
  unsigned int thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
  Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
  Value* thunk = GetElementPtrInst::Create(int8Type, bitcast, Idx, "add.ptr", basicBlock);
  return new BitCastInst(thunk, interface->getLLVMType(llvmContext), "", basicBlock);
}

int Model::getInterfaceIndex(Interface* interface) const {
  int index = 0;
  for (Interface* implementedInterface : mFlattenedInterfaceHierarchy) {
    if (implementedInterface == interface) {
      return index;
    }
    index++;
  }
  return -1;
}
