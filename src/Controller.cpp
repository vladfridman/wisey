//
//  Controller.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Cast.hpp"
#include "wisey/Controller.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterReferenceVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Controller::Controller(AccessLevel accessLevel,
                       string name,
                       StructType* structType,
                       bool isExternal) :
mAccessLevel(accessLevel),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false) {
  mControllerOwner = new ControllerOwner(this);
}

Controller::~Controller() {
  delete mControllerOwner;
  for(Field* field : mFieldsOrdered) {
    delete field;
  }
  mFieldsOrdered.clear();
  mFieldIndexes.clear();
  mFields.clear();
  mReceivedFields.clear();
  mInjectedFields.clear();
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

Controller* Controller::newController(AccessLevel accessLevel,
                                      string name,
                                      StructType* structType) {
  return new Controller(accessLevel , name, structType, false);
}

Controller* Controller::newExternalController(string name, StructType* structType) {
  return new Controller(AccessLevel::PUBLIC_ACCESS, name, structType, true);
}

AccessLevel Controller::getAccessLevel() const {
  return mAccessLevel;
}

void Controller::setFields(vector<Field*> fields, unsigned long startIndex) {
  unsigned long index = startIndex;
  for (Field* field : fields) {
    mFields[field->getName()] = field;
    mFieldsOrdered.push_back(field);
    mFieldIndexes[field] = index;
    switch (field->getFieldKind()) {
      case FieldKind::RECEIVED_FIELD :
        mReceivedFields.push_back(field);
        break;
      case FieldKind::STATE_FIELD :
        mStateFields.push_back(field);
        break;
      case FieldKind::INJECTED_FIELD :
        mInjectedFields.push_back(field);
        break;
      default:
        Log::e("Controllers can only have received, injected or state fields");
        exit(1);
        break;
    }
    index++;
  }
}

void Controller::setInterfaces(vector<Interface *> interfaces) {
  mInterfaces = interfaces;
  
  for (Interface* interface : mInterfaces) {
    IConcreteObjectType::addInterfaceAndItsParents(interface, mFlattenedInterfaceHierarchy);
  }
}

void Controller::setMethods(vector<IMethod *> methods) {
  mMethods = methods;
  for (IMethod* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

void Controller::setStructBodyTypes(vector<Type*> types) {
  mStructType->setBody(types);
}

void Controller::setConstants(vector<Constant*> constants) {
  mConstants = constants;
  for (Constant* constant : constants) {
    mNameToConstantMap[constant->getName()] = constant;
  }
}

vector<wisey::Constant*> Controller::getConstants() const {
  return mConstants;
}

wisey::Constant* Controller::findConstant(string constantName) const {
  if (!mNameToConstantMap.count(constantName)) {
    Log::e("Controller " + mName + " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

Instruction* Controller::inject(IRGenerationContext& context,
                                InjectionArgumentList injectionArgumentList,
                                int line) const {
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(this);
  
  checkArguments(injectionArgumentList);
  Instruction* malloc = createMalloc(context);
  IntrinsicFunctions::setMemoryToZero(context, malloc, ConstantExpr::getSizeOf(mStructType));
  initializeReceivedFields(context, injectionArgumentList, malloc, line);
  initializeInjectedFields(context, malloc, line);
  initializeVTable(context, (IConcreteObjectType*) this, malloc);
  
  context.setObjectType(lastObjectType);
  
  return malloc;
}

void Controller::checkArguments(const InjectionArgumentList& received) const {
  checkArgumentsAreWellFormed(received);
  checkAllFieldsAreSet(received);
}

void Controller::checkArgumentsAreWellFormed(const InjectionArgumentList&
                                             injectionArgumentList) const {
  bool areArgumentsWellFormed = true;
  
  for (InjectionArgument* argument : injectionArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(this);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some injection arguments for controller " + mName + " are not well formed");
    exit(1);
  }
}

void Controller::checkAllFieldsAreSet(const InjectionArgumentList& injectionArgumentList) const {
  set<string> allFieldsThatAreSet;
  for (InjectionArgument* argument : injectionArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingReceivedFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e("Received field " + missingField + " is not initialized");
  }
  Log::e("Some received fields of the controller " + mName + " are not initialized.");
  exit(1);
}

vector<string> Controller::getMissingReceivedFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (Field* field : mReceivedFields) {
    if (givenFields.find(field->getName()) == givenFields.end()) {
      missingFields.push_back(field->getName());
    }
  }
  
  return missingFields;
}

vector<Interface*> Controller::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Controller::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

string Controller::getTypeTableName() const {
  return mName + ".typetable";
}

string Controller::getVTableName() const {
  return mName + ".vtable";
}

unsigned long Controller::getVTableSize() const {
  return mFlattenedInterfaceHierarchy.size();
}

Instruction* Controller::createMalloc(IRGenerationContext& context) const {
  Type* structType = getLLVMType(context)->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 1);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, one, "injectvar");
  
  return malloc;
}

Field* Controller::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

unsigned long Controller::getFieldIndex(Field* field) const {
  return mFieldIndexes.at(field);
}

vector<Field*> Controller::getFields() const {
  return mFieldsOrdered;
}

IMethod* Controller::findMethod(std::string methodName) const {
  if (!mNameToMethodMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodMap.at(methodName);
}

vector<IMethod*> Controller::getMethods() const {
  return mMethods;
}

string Controller::getObjectNameGlobalVariableName() const {
  return mName + ".name";
}

string Controller::getObjectShortNameGlobalVariableName() const {
  return mName + ".shortname";
}

string Controller::getTypeName() const {
  return mName;
}

string Controller::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Controller::getLLVMType(IRGenerationContext& context) const {
  return mStructType->getPointerTo();
}

TypeKind Controller::getTypeKind() const {
  return CONTROLLER_TYPE;
}

bool Controller::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return IConcreteObjectType::canCast(this, toType);
}

bool Controller::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* Controller::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType);
}

bool Controller::isOwner() const {
  return false;
}

bool Controller::isReference() const {
  return true;
}

bool Controller::isArray() const {
  return false;
}

void Controller::initializeReceivedFields(IRGenerationContext& context,
                                          const InjectionArgumentList& controllerInjectorArguments,
                                          Instruction* malloc,
                                          int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (InjectionArgument* argument : controllerInjectorArguments) {
    string argumentName = argument->deriveFieldName();
    Field* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    const IType* fieldType = field->getType();
    
    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      Log::e("Controller injector argumet value for field '" + field->getName() +
             "' does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    IRWriter::newStoreInst(context, castValue, fieldPointer);
    if (IType::isReferenceType(fieldType)) {
      ((IObjectType*) fieldType)->incrementReferenceCount(context, castValue);
    }
  }
}

void Controller::initializeInjectedFields(IRGenerationContext& context,
                                          Instruction* malloc,
                                          int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  Value *index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (Field* field : mInjectedFields) {
    const IType* fieldType = field->getType();
    TypeKind fieldTypeKind = fieldType->getTypeKind();
    Value* fieldValue = NULL;
    if (fieldTypeKind == CONTROLLER_TYPE || fieldTypeKind == ARRAY_TYPE) {
      Log::e("Injected fields must have owner type denoted by '*'");
      exit(1);
    } else if (fieldTypeKind == ARRAY_OWNER_TYPE) {
      const ArraySpecificOwnerType* arraySpecificOwnerType =
      (const ArraySpecificOwnerType*) field->getInjectedType();
      const ArraySpecificType* arraySpecificType = arraySpecificOwnerType->getArraySpecificType();
      Value* arrayPointer = ArrayAllocation::allocateArray(context, arraySpecificType);
      fieldValue = IRWriter::newBitCastInst(context, arrayPointer, arraySpecificType->
                                            getArrayType(context)->getLLVMType(context));
    } else if (fieldTypeKind == CONTROLLER_OWNER_TYPE) {
      Controller* controller = (Controller*) ((IObjectOwnerType*) fieldType)->getObject();
      fieldValue = controller->inject(context, field->getInjectionArguments(), line);
    } else {
      Log::e("Attempt to inject a variable that is not a controller, an interface or an array");
      exit(1);
    }
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    IRWriter::newStoreInst(context, fieldValue, fieldPointer);
  }
}

const IObjectOwnerType* Controller::getOwner() const {
  return mControllerOwner;
}

bool Controller::isExternal() const {
  return mIsExternal;
}

void Controller::printToStream(IRGenerationContext& context, iostream& stream) const {
  IConcreteObjectType::printObjectToStream(context, this, stream);
}

void Controller::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, 1);
}

void Controller::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, -1);
}

Value* Controller::getReferenceCount(IRGenerationContext& context, Value* object) const {
  return getReferenceCountForObject(context, object);
}

void Controller::setImportProfile(ImportProfile* importProfile) {
  mImportProfile = importProfile;
}

ImportProfile* Controller::getImportProfile() const {
  return mImportProfile;
}

void Controller::addInnerObject(const IObjectType* innerObject) {
  mInnerObjects[innerObject->getShortName()] = innerObject;
}

const IObjectType* Controller::getInnerObject(string shortName) const {
  if (mInnerObjects.count(shortName)) {
    return mInnerObjects.at(shortName);
  }
  return NULL;
}

map<string, const IObjectType*> Controller::getInnerObjects() const {
  return mInnerObjects;
}

void Controller::markAsInner() {
  mIsInner = true;
}

bool Controller::isInner() const {
  return mIsInner;
}

Function* Controller::getReferenceAdjustmentFunction(IRGenerationContext& context) const {
  return AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(context);
}

void Controller::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void Controller::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  IVariable* variable = new FieldReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void Controller::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* Controller::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

