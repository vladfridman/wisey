//
//  Controller.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Cast.hpp"
#include "wisey/Controller.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/Environment.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Controller::Controller(string name, StructType* structType, bool isExternal) :
mName(name),
mStructType(structType),
mIsExternal(isExternal) {
  mControllerOwner = new ControllerOwner(this);
}

Controller::~Controller() {
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
}

Controller* Controller::newController(string name, StructType* structType) {
  return new Controller(name, structType, false);
}

Controller* Controller::newExternalController(string name, StructType* structType) {
  return new Controller(name, structType, true);
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
        Log::e("Controllers can only have fixed, injected or state fields");
        exit(1);
        break;
    }
    index++;
  }
}

void Controller::setInterfaces(vector<Interface *> interfaces) {
  mInterfaces = interfaces;
  
  for (Interface* interface : mInterfaces) {
    addInterfaceAndItsParents(mFlattenedInterfaceHierarchy, interface);
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

Instruction* Controller::inject(IRGenerationContext& context, ExpressionList received) const {
  checkArguments(received);
  Instruction* malloc = createMalloc(context);
  initializeReceivedFields(context, received, malloc);
  initializeInjectedFields(context, malloc);
  initializeStateFields(context, malloc);
  initializeVTable(context, (IConcreteObjectType*) this, malloc);

  return malloc;
}

void Controller::checkArguments(ExpressionList received) const {
  if (received.size() == mReceivedFields.size()) {
    return;
  }
  if (received.size() < mReceivedFields.size()) {
    Log::e("Not all received fields of controller " + mName + " are initialized.");
    exit(1);
  }
  Log::e("Too many arguments provided when injecting controller " + mName);
  exit(1);
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
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* structType = getLLVMType(llvmContext)->getPointerElementType()->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, "injectvar");
  
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

string Controller::getName() const {
  return mName;
}

string Controller::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Controller::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType->getPointerTo()->getPointerTo();
}

TypeKind Controller::getTypeKind() const {
  return CONTROLLER_TYPE;
}

bool Controller::canCastTo(const IType* toType) const {
  if (toType == this) {
    return true;
  }
  if (toType->getTypeKind() == INTERFACE_TYPE &&
      getInterfaceIndex((IConcreteObjectType*) this, (Interface*) toType) >= 0) {
    return true;
  }
  return false;
}

bool Controller::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* Controller::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType);
}

void Controller::addInterfaceAndItsParents(vector<Interface*>& result, Interface* interface) const {
  result.push_back(interface);
  vector<Interface*> parentInterfaces = interface->getParentInterfaces();
  for (Interface* interface : parentInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
}

void Controller::initializeReceivedFields(IRGenerationContext& context,
                                          ExpressionList& controllerInjectorArguments,
                                          Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  unsigned int fieldIndex = 0;
  for (IExpression* argument : controllerInjectorArguments) {
    Value* fieldValue = argument->generateIR(context);
    const IType* fieldType = argument->getType(context);
    Field* field = mReceivedFields[fieldIndex];
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    if (field->getType() != fieldType) {
      Log::e("Controller injector argumet value for field '" + field->getName() +
             "' does not match its type");
      exit(1);
    }
    if (fieldType->getTypeKind() == PRIMITIVE_TYPE) {
      IRWriter::newStoreInst(context, fieldValue, fieldPointer);
    } else {
      Value* fieldValueLoaded = IRWriter::newLoadInst(context, fieldValue, "");
      IRWriter::newStoreInst(context, fieldValueLoaded, fieldPointer);
    }
    fieldIndex++;
  }
}

void Controller::initializeInjectedFields(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  Value *index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (Field* field : mInjectedFields) {
    const IType* fieldType = field->getType();
    if (fieldType->getTypeKind() == CONTROLLER_TYPE) {
      Log::e("Injected fields must have owner type denoted by '*'");
      exit(1);
    }
    if (fieldType->getTypeKind() != CONTROLLER_OWNER_TYPE) {
      Log::e("Attempt to inject a variable that is not a Controller or an Interface");
      exit(1);
    }
    Controller* controller = (Controller*) ((IObjectOwnerType*) fieldType)->getObject();
    Value* fieldValue = controller->inject(context, field->getArguments());
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    IRWriter::newStoreInst(context, fieldValue, fieldPointer);
  }
}

void Controller::initializeStateFields(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value *index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (Field* field : mStateFields) {
    const IType* fieldType = field->getType();
    Type* fieldLLVMType = fieldType->getLLVMType(llvmContext);
    
    Value* fieldValue;
    if (IType::isOwnerType(fieldType)) {
      fieldValue = ConstantPointerNull::get((PointerType*) fieldType->getLLVMType(llvmContext));
    } else if (IType::isReferenceType(fieldType)) {
      fieldValue = ConstantPointerNull::get((PointerType*) fieldType->getLLVMType(llvmContext)
                                            ->getPointerElementType());
    } else if (fieldLLVMType->isFloatTy() || fieldLLVMType->isDoubleTy()) {
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
      Log::e("Unexpected controller state field type, can not initialize");
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
