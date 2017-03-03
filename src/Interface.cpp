//
//  Interface.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Cast.hpp"
#include "yazyk/Interface.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Interface::~Interface() {
  mMethods->clear();
  
  delete mMethods;
}

Method* Interface::findMethod(std::string methodName) const {
  if (!mMethods->count(methodName)) {
    return NULL;
  }
  
  return mMethods->at(methodName);
}

void Interface::generateMapFunctionsIR(IRGenerationContext& context,
                                       Model* model,
                                       vector<Constant*>& vtableArray,
                                       int interfaceIndex) const {
  for (map<string, Method*>::iterator iterator = mMethods->begin();
       iterator != mMethods->end();
       iterator++) {
    Method* method = iterator->second;
    generateMapFunctionForMethod(context, model, vtableArray, interfaceIndex, method);
  }
}

void Interface::generateMapFunctionForMethod(IRGenerationContext& context,
                                             Model* model,
                                             vector<Constant*>& vtableArray,
                                             int interfaceIndex,
                                             Method* method) const {
  Method* modelMethod = model->findMethod(method->getName());
  if (modelMethod == NULL) {
    Log::e("Method '" + method->getName() + "' of interface '" + mName +
           "' is not implemented by model '" + model->getName() + "'");
    exit(1);
  }
  
  if (!modelMethod->equals(method)) {
    Log::e("Method '" + method->getName() + "' of interface '" + mName +
           "' has different argument types when implmeneted by model '"
           + model->getName() + "'");
    exit(1);
  }
}

string Interface::getName() const {
  return mName;
}

llvm::Type* Interface::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType;
}

TypeKind Interface::getTypeKind() const {
  return INTERFACE_TYPE;
}

bool Interface::canCastTo(IType* toType) const {
  // TODO: implement casting
  return false;
}

bool Interface::canCastLosslessTo(IType* toType) const {
  // TODO: implement casting
  return false;
}

Value* Interface::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  // TODO: implement casting
  return NULL;
}
