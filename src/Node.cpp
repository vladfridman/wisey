//
//  Node.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/NodeOwner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Node.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Node::Node(string name, StructType* structType, bool isExternal) :
mName(name),
mStructType(structType),
mIsExternal(isExternal) {
  mNodeOwner = new NodeOwner(this);
}

Node::~Node() {
  for(Field* field : mFieldsOrdered) {
    delete field;
  }
  mFieldsOrdered.clear();
  mFields.clear();
  mFixedFields.clear();
  mStateFields.clear();
  for (IMethod* method : mMethods) {
    delete method;
  }
  mMethods.clear();
  mNameToMethodMap.clear();
  mInterfaces.clear();
  mFlattenedInterfaceHierarchy.clear();
}

Node* Node::newNode(string name, StructType* structType) {
  return new Node(name, structType, false);
}

Node* Node::newExternalNode(string name, StructType* structType) {
  return new Node(name, structType, true);
}

void Node::setFields(vector<Field*> fields) {
  mFieldsOrdered = fields;
  for (Field* field : fields) {
    mFields[field->getName()] = field;
    switch (field->getFieldKind()) {
      case FieldKind::FIXED_FIELD :
        mFixedFields.push_back(field);
        break;
      case FieldKind::STATE_FIELD :
        mStateFields.push_back(field);
        break;
      default:
        break;
    }
  }
}

void Node::setInterfaces(vector<Interface *> interfaces) {
  mInterfaces = interfaces;
  
  for (Interface* interface : mInterfaces) {
    addInterfaceAndItsParents(mFlattenedInterfaceHierarchy, interface);
  }
}

void Node::addInterfaceAndItsParents(vector<Interface*>& result, Interface* interface) const {
  result.push_back(interface);
  vector<Interface*> parentInterfaces = interface->getParentInterfaces();
  for (Interface* interface : parentInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
}

void Node::setMethods(vector<IMethod *> methods) {
  mMethods = methods;
  for (IMethod* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

void Node::setStructBodyTypes(vector<Type*> types) {
  mStructType->setBody(types);
}

Field* Node::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

vector<Field*> Node::getFields() const {
  return mFieldsOrdered;
}

IMethod* Node::findMethod(string methodName) const {
  if (!mNameToMethodMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodMap.at(methodName);
}

vector<IMethod*> Node::getMethods() const {
  return mMethods;
}

string Node::getObjectNameGlobalVariableName() const {
  return mName + ".name";
}

string Node::getName() const {
  return mName;
}

string Node::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

PointerType* Node::getLLVMType(LLVMContext& llvmcontext) const {
  return mStructType->getPointerTo()->getPointerTo();
}

TypeKind Node::getTypeKind() const {
  return NODE_TYPE;
}

bool Node::canCastTo(const IType* toType) const {
  if (toType == this) {
    return true;
  }
  if (toType->getTypeKind() == INTERFACE_TYPE &&
      getInterfaceIndex((IConcreteObjectType*) this, (Interface*) toType) >= 0) {
    return true;
  }
  return false;
}

bool Node::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* Node::castTo(IRGenerationContext& context, Value* fromValue, const IType* toType) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType);
}

string Node::getVTableName() const {
  return mName + ".vtable";
}

unsigned long Node::getVTableSize() const {
  return mFlattenedInterfaceHierarchy.size();
}

vector<Interface*> Node::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Node::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

string Node::getTypeTableName() const {
  return mName + ".typetable";
}

const IObjectOwnerType* Node::getOwner() const {
  return mNodeOwner;
}

vector<string> Node::getMissingFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (Field* fixedField : mFixedFields) {
    if (givenFields.find(fixedField->getName()) == givenFields.end()) {
      missingFields.push_back(fixedField->getName());
    }
  }
  
  return missingFields;
}

Instruction* Node::build(IRGenerationContext& context,
                        const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const {
  checkArguments(ObjectBuilderArgumentList);
  Instruction* malloc = createMalloc(context);
  setStateFieldsToNull(context, malloc);
  initializePresetFields(context, ObjectBuilderArgumentList, malloc);
  initializeVTable(context, (IConcreteObjectType*) this, malloc);
  
  return malloc;
}

void Node::checkArguments(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const {
  checkArgumentsAreWellFormed(ObjectBuilderArgumentList);
  checkAllFieldsAreSet(ObjectBuilderArgumentList);
}

void Node::checkArgumentsAreWellFormed(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const {
  bool areArgumentsWellFormed = true;
  
  for (ObjectBuilderArgument* argument : ObjectBuilderArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(this);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some arguments for the node " + mName + " builder are not well formed");
    exit(1);
  }
}

void Node::checkAllFieldsAreSet(const ObjectBuilderArgumentList& objectBuilderArgumentList) const {
  set<string> allFieldsThatAreSet;
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e("Field " + missingField + " of the node " + mName + " is not initialized.");
  }
  Log::e("Some fields of the node " + mName + " are not initialized.");
  exit(1);
}

Instruction* Node::createMalloc(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* structType = getLLVMType(llvmContext)->getPointerElementType()->getPointerElementType();
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, "buildervar");
  
  return malloc;
}

void Node::initializePresetFields(IRGenerationContext& context,
                                  const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                  Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    string argumentName = argument->deriveFieldName();
    Value* argumentValue = argument->getValue(context);
    const IType* argumentType = argument->getType(context);
    Field* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    if (!argumentType->canAutoCastTo(field->getType())) {
      Log::e("Node builder argument value for field " + argumentName +
             " does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, field->getType());
    if (argumentType->getTypeKind() == PRIMITIVE_TYPE) {
      IRWriter::newStoreInst(context, castValue, fieldPointer);
    } else {
      Value* argumentValueLoaded = IRWriter::newLoadInst(context, castValue, "");
      IRWriter::newStoreInst(context, argumentValueLoaded, fieldPointer);
    }
    if (IType::isOwnerType(field->getType())) {
      argument->releaseOwnership(context);
    }
  }
}

void Node::setStateFieldsToNull(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value *index[2];
  index[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  
  for (Field* field : mStateFields) {
    const IType* fieldType = field->getType();
    Type* fieldLLVMType = fieldType->getLLVMType(llvmContext);
    
    Value* fieldValue;
    if (IType::isOwnerType(fieldType)) {
      fieldValue = ConstantPointerNull::get((PointerType*) fieldType->getLLVMType(llvmContext));
    } else if (IType::isReferenceType(fieldType)) {
      fieldValue = ConstantPointerNull::get((PointerType*) fieldType->getLLVMType(llvmContext)
                                            ->getPointerElementType());
    } else if (fieldLLVMType->isFloatTy()) {
      fieldValue = ConstantFP::get(fieldLLVMType, 0.0);
    } else if (fieldLLVMType->isIntegerTy()) {
      fieldValue = ConstantInt::get(fieldLLVMType, 0);
    } else {
      Log::e("Unexpected state field type, can not initialize");
      exit(1);
    }
    
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    IRWriter::newStoreInst(context, fieldValue, fieldPointer);
  }
}

bool Node::isExternal() const {
  return mIsExternal;
}

void Node::printToStream(IRGenerationContext& context, iostream& stream) const {
  IConcreteObjectType::printObjectToStream(context, this, stream);
}
