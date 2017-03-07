//
//  Model.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Cast.hpp"
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

ModelField* Model::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

vector<string> Model::getMissingFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (map<string, ModelField*>::const_iterator iterator = mFields.begin();
       iterator != mFields.end();
       iterator++) {
    string modelFieldName = iterator->first;
    if (givenFields.find(modelFieldName) == givenFields.end()) {
      missingFields.push_back(modelFieldName);
    }
  }
  
  return missingFields;
}

Method* Model::findMethod(std::string methodName) const {
  if (!mMethods.count(methodName)) {
    return NULL;
  }
  
  return mMethods.at(methodName);
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
  if (toType->getTypeKind() == INTERFACE_TYPE && getInterfaceIndex((Interface*) toType) >= 0) {
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
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* basicBlock = context.getBasicBlock();
  Interface* interface = (Interface*) toType;
  int interfaceIndex = getInterfaceIndex(interface);
  if (interfaceIndex == 0) {
    return new BitCastInst(fromValue, interface->getLLVMType(llvmContext), "", basicBlock);
  }
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* bitcast = new BitCastInst(fromValue, int8Type->getPointerTo(), "", basicBlock);
  Value *Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 8 * interfaceIndex);
  Value* thunk = GetElementPtrInst::Create(int8Type, bitcast, Idx, "add.ptr", basicBlock);
  return new BitCastInst(thunk, interface->getLLVMType(llvmContext), "", basicBlock);
}

int Model::getInterfaceIndex(Interface* interface) const {
  int index = 0;
  for (Interface* implementedInterface : mInterfaces) {
    if (implementedInterface == interface) {
      return index;
    }
    index++;
  }
  return -1;
}
