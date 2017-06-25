//
//  Node.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/NodeOwner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Node.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Node::Node(string name, StructType* structType) {
  mName = name;
  mStructType = structType;
  mNodeOwner = new NodeOwner(this);
}

Node::~Node() {
  for (Field* field : mFixedFields) {
    delete field;
  }
  mFixedFields.clear();
  for (Field* field : mStateFields) {
    delete field;
  }
  mStateFields.clear();
  for (Method* method : mMethods) {
    delete method;
  }
  mMethods.clear();
  mFields.clear();
  mNameToMethodMap.clear();
  mInterfaces.clear();
  mFlattenedInterfaceHierarchy.clear();
}

Field* Node::findField(string fieldName) const {
  // TODO: implement this
  return NULL;
}

map<string, Field*> Node::getFields() const {
  // TODO: test this
  return mFields;
}

Method* Node::findMethod(string methodName) const {
  // TODO: implement this
  return NULL;
}

vector<Method*> Node::getMethods() const {
  // TODO: test this
  return mMethods;
}

string Node::getObjectNameGlobalVariableName() const {
  // TODO: implement this
  return "";
}

string Node::getName() const {
  // TODO: test this
  return mName;
}

string Node::getShortName() const {
  // TODO: implement this
  return "";
}

PointerType* Node::getLLVMType(LLVMContext& llvmcontext) const {
  // TODO: implement this
  return NULL;
}

TypeKind Node::getTypeKind() const {
  return NODE_TYPE;
}

bool Node::canCastTo(const IType* toType) const {
  // TODO: implement this
  return false;
}

bool Node::canAutoCastTo(const IType* toType) const {
  // TODO: implement this
  return false;
}

Value* Node::castTo(IRGenerationContext& context, Value* fromValue, const IType* toType) const {
  // TODO: implement this
  return NULL;
}

string Node::getVTableName() const {
  // TODO: implement this
  return "";
}

unsigned long Node::getVTableSize() const {
  // TODO: implement this
  return 0;
}

vector<Interface*> Node::getInterfaces() const {
  // TODO: test this
  return mInterfaces;
}

vector<Interface*> Node::getFlattenedInterfaceHierarchy() const {
  // TODO: test this
  return mFlattenedInterfaceHierarchy;
}

string Node::getTypeTableName() const {
  // TODO: implement this
  return "";
}

IObjectOwnerType* Node::getOwner() const {
  return mNodeOwner;
}


