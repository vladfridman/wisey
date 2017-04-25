//
//  Controller.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Controller.hpp"
#include "yazyk/Environment.hpp"
#include "yazyk/Field.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"

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

Instruction* Controller::inject(IRGenerationContext& context, ExpressionList received) const {
  checkArguments(received);
  Instruction* malloc = createMalloc(context);
  initializeReceivedFields(context, received, malloc);
  initializeInjectedFields(context, malloc);
  initializeStateFields(context, malloc);
  initializeVTable(context, malloc);
  
  return malloc;
}

void Controller::checkArguments(ExpressionList received) const {
  if (received.size() == mReceivedFields.size()) {
    return;
  }
  if (received.size() < mReceivedFields.size()) {
    Log::e("Not all received fields of controller '" + mName + "' are initialized.");
    exit(1);
  }
  Log::e("Too many arguments provided when injecting controller '" + mName + "'");
  exit(1);
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

Instruction* Controller::createMalloc(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* pointerType = Type::getInt32Ty(llvmContext);
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, pointerType);
  Instruction* malloc = CallInst::CreateMalloc(context.getBasicBlock(),
                                               pointerType,
                                               structType,
                                               allocSize,
                                               nullptr,
                                               nullptr,
                                               "injectvar");
  context.getBasicBlock()->getInstList().push_back(malloc);
  
  return malloc;
}

void Controller::initializeVTable(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* basicBlock = context.getBasicBlock();
  GlobalVariable* vTableGlobal = context.getModule()->getGlobalVariable(getVTableName());
  
  Type* genericPointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vTableType = functionType->getPointerTo()->getPointerTo();
  
  vector<Interface*> interfaces = getFlattenedInterfaceHierarchy();
  for (unsigned int interfaceIndex = 0; interfaceIndex < interfaces.size(); interfaceIndex++) {
    Value* vTableStart;
    if (interfaceIndex == 0) {
      vTableStart = malloc;
    } else {
      Value* vTableStartCalculation = new BitCastInst(malloc, genericPointerType, "", basicBlock);
      Value *Idx[1];
      unsigned int thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
      Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
      vTableStart = GetElementPtrInst::Create(genericPointerType->getPointerElementType(),
                                              vTableStartCalculation,
                                              Idx,
                                              "",
                                              basicBlock);
    }
    
    Value* vTablePointer = new BitCastInst(vTableStart, vTableType->getPointerTo(), "", basicBlock);
    Value *Idx[3];
    Idx[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), interfaceIndex);
    Idx[2] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Value* initializerStart = GetElementPtrInst::Create(vTableGlobal->getType()->
                                                        getPointerElementType(),
                                                        vTableGlobal,
                                                        Idx,
                                                        "",
                                                        basicBlock);
    BitCastInst* bitcast = new BitCastInst(initializerStart, vTableType, "", basicBlock);
    new StoreInst(bitcast, vTablePointer, basicBlock);
  }
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

void Controller::initializeReceivedFields(IRGenerationContext& context,
                                          ExpressionList& controllerInjectorArguments,
                                          Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();
  
  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  unsigned int fieldIndex = 0;
  for (IExpression* argument : controllerInjectorArguments) {
    Value* fieldValue = argument->generateIR(context);
    IType* fieldType = argument->getType(context);
    Field* field = mReceivedFields[fieldIndex];
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    GetElementPtrInst* fieldPointer =
    GetElementPtrInst::Create(structType, malloc, Idx, "", context.getBasicBlock());
    if (field->getType() != fieldType) {
      Log::e("Controller injector argumet value for field '" + field->getName() +
             "' does not match its type");
      exit(1);
    }
    new StoreInst(fieldValue, fieldPointer, context.getBasicBlock());
    fieldIndex++;
  }
}

void Controller::initializeInjectedFields(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();

  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (Field* field : mInjectedFields) {
    IType* fieldType = field->getType();
    if (fieldType->getTypeKind() != CONTROLLER_TYPE) {
      Log::e("Attempt to inject a variable that is not a Controller");
      exit(1);
    }
    Controller* controller = dynamic_cast<Controller*>(fieldType);
    Value* fieldValue = controller->inject(context, field->getArguments());
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    GetElementPtrInst* fieldPointer =
    GetElementPtrInst::Create(structType, malloc, Idx, "", context.getBasicBlock());
    new StoreInst(fieldValue, fieldPointer, context.getBasicBlock());
  }
}

void Controller::initializeStateFields(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();
  
  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (Field* field : mStateFields) {
    IType* fieldType = field->getType();
    Type* fieldLLVMType = fieldType->getLLVMType(llvmContext);
    
    Value* fieldValue;
    if (fieldLLVMType->isPointerTy()) {
      fieldValue = ConstantPointerNull::get((PointerType*) fieldType->getLLVMType(llvmContext));
    } else if (fieldLLVMType->isFloatTy()) {
      fieldValue = ConstantFP::get(fieldLLVMType, 0.0);
    } else if (fieldLLVMType->isIntegerTy()) {
      fieldValue = ConstantInt::get(fieldLLVMType, 0);
    } else {
      Log::e("Unexpected state field type, can not initialize");
      exit(1);
    }
    
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    GetElementPtrInst* fieldPointer =
    GetElementPtrInst::Create(structType, malloc, Idx, "", context.getBasicBlock());
    new StoreInst(fieldValue, fieldPointer, context.getBasicBlock());
  }
 
}