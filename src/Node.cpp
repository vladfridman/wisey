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
#include "wisey/CheckCastToObjectFunction.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/Node.hpp"
#include "wisey/NodeOwner.hpp"
#include "wisey/ParameterReferenceVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Node::Node(AccessLevel accessLevel,
           string name,
           StructType* structType,
           ImportProfile* importProfile,
           bool isExternal,
           int line) :
mIsPublic(accessLevel == PUBLIC_ACCESS),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false),
mNodeOwner(new NodeOwner(this)),
mImportProfile(importProfile),
mLine(line) {
  assert(importProfile && "Import profile can not be NULL at Node creation");
}

Node::~Node() {
  delete mNodeOwner;
  for(IField* field : mFieldsOrdered) {
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
  for (LLVMFunction* llvmFunction : mLLVMFunctions) {
    delete llvmFunction;
  }
  mLLVMFunctions.clear();
}

Node* Node::newNode(AccessLevel accessLevel,
                    string name,
                    StructType* structType,
                    ImportProfile* importProfile,
                    int line) {
  return new Node(accessLevel, name, structType, importProfile, false, line);
}

Node* Node::newExternalNode(string name,
                            StructType* structType,
                            ImportProfile* importProfile,
                            int line) {
  return new Node(AccessLevel::PUBLIC_ACCESS, name, structType, importProfile, true, line);
}

bool Node::isPublic() const {
  return mIsPublic;
}

void Node::setFields(IRGenerationContext& context,
                     vector<IField*> fields,
                     unsigned long startIndex) {
  mFieldsOrdered = fields;
  unsigned long index = startIndex;
  for (IField* field : fields) {
    mFields[field->getName()] = field;
    mFieldIndexes[field] = index;

    if (field->isFixed()) {
      mFixedFields.push_back(field);
    } else if (field->isState()) {
      mStateFields.push_back(field);
    } else {
      context.reportError(field->getLine(),
                          "Nodes can only have fixed or state fields");
      throw 1;
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

wisey::Constant* Node::findConstant(IRGenerationContext& context,
                                    string constantName,
                                    int line) const {
  if (!mNameToConstantMap.count(constantName)) {
    context.reportError(line, "Node " + mName + " does not have constant named " + constantName);
    throw 1;
  }
  return mNameToConstantMap.at(constantName);
}

vector<wisey::Constant*> Node::getConstants() const {
  return mConstants;
}

void Node::setLLVMFunctions(vector<LLVMFunction*> llvmFunctions) {
  mLLVMFunctions = llvmFunctions;
  for (LLVMFunction* function : llvmFunctions) {
    mLLVMFunctionMap[function->getName()] = function;
  }
}

vector<LLVMFunction*> Node::getLLVMFunctions() const {
  return mLLVMFunctions;
}

LLVMFunction* Node::findLLVMFunction(string functionName) const {
  if (mLLVMFunctionMap.count(functionName)) {
    return mLLVMFunctionMap.at(functionName);
  }
  return NULL;
}

IField* Node::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

unsigned long Node::getFieldIndex(const IField* field) const {
  return mFieldIndexes.at(field);
}

vector<IField*> Node::getFields() const {
  return mFieldsOrdered;
}

IMethod* Node::findMethod(string methodName) const {
  return IConcreteObjectType::findMethodInObject(methodName, this);
}

vector<IMethod*> Node::getMethods() const {
  return mMethods;
}

map<string, IMethod*> Node::getNameToMethodMap() const {
  return mNameToMethodMap;
}

string Node::getObjectNameGlobalVariableName() const {
  return mName + ".name";
}

string Node::getObjectShortNameGlobalVariableName() const {
  return mName + ".shortname";
}

string Node::getTypeName() const {
  return mName;
}

string Node::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Node::getLLVMType(IRGenerationContext& context) const {
  return mStructType->getPointerTo();
}

bool Node::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->isReference()) {
    return !toType->isController() && !toType->isModel();
  }
  return IConcreteObjectType::canCast(this, toType);
}

bool Node::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* Node::castTo(IRGenerationContext& context,
                    Value* fromValue,
                    const IType* toType,
                    int line) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType, line);
}

bool Node::isPrimitive() const {
  return false;
}

bool Node::isOwner() const {
  return false;
}

bool Node::isReference() const {
  return true;
}

bool Node::isArray() const {
  return false;
}

bool Node::isFunction() const {
  return false;
}

bool Node::isController() const {
  return false;
}

bool Node::isInterface() const {
  return false;
}

bool Node::isModel() const {
  return false;
}

bool Node::isNode() const {
  return true;
}

bool Node::isNative() const {
  return false;
}

bool Node::isPackage() const {
  return false;
}

bool Node::isPointer() const {
  return false;
}

bool Node::isImmutable() const {
  return false;
}

string Node::getVTableName() const {
  return mName + ".vtable";
}

unsigned long Node::getVTableSize() const {
  return IConcreteObjectType::getVTableSizeForObject(this);
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
  
  for (IField* fixedField : mFixedFields) {
    if (givenFields.find(fixedField->getName()) == givenFields.end()) {
      missingFields.push_back(fixedField->getName());
    }
  }
  
  return missingFields;
}

Instruction* Node::build(IRGenerationContext& context,
                         const ObjectBuilderArgumentList& objectBuilderArgumentList,
                         int line) const {
  checkArguments(context, objectBuilderArgumentList, line);
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context, this, "buildervar");
  initializePresetFields(context, objectBuilderArgumentList, malloc, line);
  initializeVTable(context, (IConcreteObjectType*) this, malloc);
  
  return malloc;
}

void Node::checkArguments(IRGenerationContext& context,
                          const ObjectBuilderArgumentList& objectBuilderArgumentList,
                          int line) const {
  checkArgumentsAreWellFormed(context, objectBuilderArgumentList, line);
  checkAllFieldsAreSet(context, objectBuilderArgumentList, line);
}

void Node::checkArgumentsAreWellFormed(IRGenerationContext& context,
                                       const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                       int line) const {
  bool areArgumentsWellFormed = true;
  
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(context, this, line);
  }
  
  if (!areArgumentsWellFormed) {
    context.reportError(line, "Some arguments for the node " + mName +
                        " builder are not well formed");
    throw 1;
  }
}

void Node::checkAllFieldsAreSet(IRGenerationContext& context,
                                const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                int line) const {
  set<string> allFieldsThatAreSet;
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    context.reportError(line, "Field " + missingField + " of the node " + mName +
                        " is not initialized.");
  }
  context.reportError(line, "Some fixed fields of the node " + mName + " are not initialized.");
  throw 1;
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
    IField* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    const IType* fieldType = field->getType();

    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      context.reportError(line, "Node builder argument value for field " + argumentName +
                          " does not match its type");
      throw 1;
    }
    if (field->isFixed() && (argumentType->isController() || argumentType->isNode())) {
      context.reportError(line, "Attempting to initialize a node fixed field with a mutable type. "
                          "Node fixed fields can only contain primitives, other models or "
                          "immutable arrays");
      throw 1;
    }
    if (field->isState() && (argumentType->isController() || argumentType->isModel())) {
      context.reportError(line, "Trying to initialize a node state field with object that is "
                          "not a node. Node state fields can only be of node owner type");
      throw 1;
    }
    if (field->isFixed() && argumentType->isInterface() && fieldType->isInterface()) {
      string typeName = context.getObjectType()->getTypeName();
      CheckCastToObjectFunction::callCheckCastToModel(context, argumentValue);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    IRWriter::newStoreInst(context, castValue, fieldPointer);
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, castValue);
    }
  }
}

bool Node::isExternal() const {
  return mIsExternal;
}

void Node::printToStream(IRGenerationContext& context, iostream& stream) const {
  IConcreteObjectType::printObjectToStream(context, this, stream);
}

void Node::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, 1);
}

void Node::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, -1);
}

Value* Node::getReferenceCount(IRGenerationContext& context, Value* object) const {
  return getReferenceCountForObject(context, object);
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

Function* Node::getReferenceAdjustmentFunction(IRGenerationContext& context) const {
  return AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(context);
}

void Node::createLocalVariable(IRGenerationContext& context,
                               string name,
                               int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, uninitializedVariable);
}

void Node::createFieldVariable(IRGenerationContext& context,
                               string name,
                               const IConcreteObjectType* object,
                               int line) const {
  IVariable* variable = new FieldReferenceVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void Node::createParameterVariable(IRGenerationContext& context,
                                   string name,
                                   Value* value,
                                   int line) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value, line);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* Node::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

Instruction* Node::inject(IRGenerationContext& context,
                          const InjectionArgumentList injectionArgumentList,
                          int line) const {
  repotNonInjectableType(context, this, line);
  throw 1;
}

int Node::getLine() const {
  return mLine;
}
