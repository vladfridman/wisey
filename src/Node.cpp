//
//  Node.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/Node.hpp"
#include "wisey/NodeOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Node::Node(AccessLevel accessLevel, string name, StructType* structType, bool isExternal) :
mAccessLevel(accessLevel),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false) {
  mNodeOwner = new NodeOwner(this);
}

Node::~Node() {
  for(Field* field : mFieldsOrdered) {
    delete field;
  }
  mFieldsOrdered.clear();
  mFieldIndexes.clear();
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
  for (Constant* constant : mConstants) {
    delete constant;
  }
  mConstants.clear();
  mNameToConstantMap.clear();
  mInnerObjects.clear();
}

Node* Node::newNode(AccessLevel accessLevel, string name, StructType* structType) {
  return new Node(accessLevel, name, structType, false);
}

Node* Node::newExternalNode(string name, StructType* structType) {
  return new Node(AccessLevel::PUBLIC_ACCESS, name, structType, true);
}

AccessLevel Node::getAccessLevel() const {
  return mAccessLevel;
}

void Node::setFields(vector<Field*> fields, unsigned long startIndex) {
  mFieldsOrdered = fields;
  unsigned long index = startIndex;
  for (Field* field : fields) {
    mFields[field->getName()] = field;
    mFieldIndexes[field] = index;
    TypeKind typeKind = field->getType()->getTypeKind();
    if (typeKind == CONTROLLER_TYPE || typeKind == CONTROLLER_OWNER_TYPE) {
      Log::e("Nodes can only have fields of primitive or model or node type");
      exit(1);
    }
    switch (field->getFieldKind()) {
      case FieldKind::FIXED_FIELD :
        mFixedFields.push_back(field);
        break;
      case FieldKind::STATE_FIELD :
        if (typeKind != NODE_OWNER_TYPE && typeKind != INTERFACE_OWNER_TYPE) {
          Log::e("Node state fields can only be node owner or interface owner type");
          exit(1);
        }
        mStateFields.push_back(field);
        break;
      default:
        Log::e("Nodes can only have fixed or state fields");
        exit(1);
        break;
    }
    index++;
  }
}

void Node::setInterfaces(vector<Interface *> interfaces) {
  mInterfaces = interfaces;
  
  for (Interface* interface : mInterfaces) {
    IConcreteObjectType::addInterfaceAndItsParents(interface, mFlattenedInterfaceHierarchy);
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

void Node::setConstants(vector<Constant*> constants) {
  mConstants = constants;
  for (Constant* constant : constants) {
    mNameToConstantMap[constant->getName()] = constant;
  }
}

wisey::Constant* Node::findConstant(string constantName) const {
  if (!mNameToConstantMap.count(constantName)) {
    Log::e("Node " + mName + " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

vector<wisey::Constant*> Node::getConstants() const {
  return mConstants;
}

Field* Node::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

unsigned long Node::getFieldIndex(Field* field) const {
  return mFieldIndexes.at(field);
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
  return mStructType->getPointerTo();
}

TypeKind Node::getTypeKind() const {
  return NODE_TYPE;
}

bool Node::canCastTo(const IType* toType) const {
  return IConcreteObjectType::canCast(this, toType);
}

bool Node::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* Node::castTo(IRGenerationContext& context,
                    Value* fromValue,
                    const IType* toType,
                    int line) const {
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
                         const ObjectBuilderArgumentList& objectBuilderArgumentList,
                         int line) const {
  checkArguments(objectBuilderArgumentList);
  Instruction* malloc = createMalloc(context);
  IConcreteObjectType::initializeReferenceCounter(context, malloc);
  setStateFieldsToNull(context, malloc);
  initializePresetFields(context, objectBuilderArgumentList, malloc, line);
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
  
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, "buildervar");
  
  return malloc;
}

void Node::initializePresetFields(IRGenerationContext& context,
                                  const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                  Instruction* malloc,
                                  int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    string argumentName = argument->deriveFieldName();
    Field* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    const IType* fieldType = field->getType();

    IRGenerationFlag irGenerationFlag = IType::isOwnerType(fieldType)
    ? IR_GENERATION_RELEASE : IR_GENERATION_NORMAL;
    Value* argumentValue = argument->getValue(context, irGenerationFlag);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(fieldType)) {
      Log::e("Node builder argument value for field " + argumentName +
             " does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    IRWriter::newStoreInst(context, castValue, fieldPointer);
    if (IType::isReferenceType(fieldType)) {
      ((IObjectType*) fieldType)->incremenetReferenceCount(context, castValue);
    }
  }
}

void Node::setStateFieldsToNull(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value *index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  
  for (Field* field : mStateFields) {
    const IType* fieldType = field->getType();
    Type* fieldLLVMType = fieldType->getLLVMType(llvmContext);
    
    Value* fieldValue;
    if (IType::isOwnerType(fieldType) || IType::isReferenceType(fieldType)) {
      fieldValue = ConstantPointerNull::get((PointerType*) fieldType->getLLVMType(llvmContext));
    } else if (fieldLLVMType->isFloatTy()) {
      fieldValue = ConstantFP::get(fieldLLVMType, 0.0);
    } else if (fieldLLVMType->isIntegerTy()) {
      fieldValue = ConstantInt::get(fieldLLVMType, 0);
    } else if (fieldLLVMType->isPointerTy()) {
      GlobalVariable* nameGlobal =
      context.getModule()->getNamedGlobal(Names::getEmptyStringName());
      ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
      Value* Idx[2];
      Idx[0] = zeroInt32;
      Idx[1] = zeroInt32;
      Type* elementType = nameGlobal->getType()->getPointerElementType();
      
      fieldValue = ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
    } else {
      Log::e("Unexpected node state field type, can not initialize");
      exit(1);
    }
    
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
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

void Node::incremenetReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, 1);
}

void Node::decremenetReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, -1);
}

Value* Node::getReferenceCount(IRGenerationContext& context, Value* object) const {
  return getReferenceCountForObject(context, object);
}

void Node::setImportProfile(ImportProfile* importProfile) {
  mImportProfile = importProfile;
}

ImportProfile* Node::getImportProfile() const {
  return mImportProfile;
}

void Node::addInnerObject(const IObjectType* innerObject) {
  mInnerObjects[innerObject->getShortName()] = innerObject;
}

const IObjectType* Node::getInnerObject(string shortName) const {
  if (mInnerObjects.count(shortName)) {
    return mInnerObjects.at(shortName);
  }
  return NULL;
}

map<string, const IObjectType*> Node::getInnerObjects() const {
  return mInnerObjects;
}

void Node::markAsInner() {
  mIsInner = true;
}

bool Node::isInner() const {
  return mIsInner;
}
