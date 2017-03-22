//
//  Interface.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Cast.hpp"
#include "yazyk/Environment.hpp"
#include "yazyk/Interface.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodCall.hpp"
#include "yazyk/MethodSignature.hpp"
#include "yazyk/Model.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Interface::~Interface() {
  mParentInterfaces.clear();
  mNameToMethodSignatureMap.clear();
}

Interface::Interface(string name,
                     StructType* structType,
                     vector<Interface*> parentInterfaces,
                     vector<MethodSignature*> methodSignatures) {
  mName = name;
  mStructType = structType;
  mParentInterfaces = parentInterfaces;
  mMethodSignatures = methodSignatures;
  
  prepare();
}

void Interface::prepare() {
  for (MethodSignature* methodSignature : mMethodSignatures) {
    mNameToMethodSignatureMap[methodSignature->getName()] = methodSignature;
    mAllMethodSignatures.push_back(methodSignature);
  }

  unsigned long methodIndex = mMethodSignatures.size();
  for (Interface* parentInterface : mParentInterfaces) {
    methodIndex = includeInterfaceMethods(parentInterface, methodIndex);
  }
}

unsigned long Interface::includeInterfaceMethods(Interface* parentInterface,
                                                 unsigned long methodIndex) {
  vector<MethodSignature*> inheritedMethods = parentInterface->getAllMethodSignatures();
  for (MethodSignature* methodSignature : inheritedMethods) {
    MethodSignature* existingMethod = findMethod(methodSignature->getName());
    if (existingMethod && !IMethodDescriptor::compare(existingMethod, methodSignature)) {
      Log::e("Interface '" + mName + "' overrides method '" + existingMethod->getName()
             + "' of parent interface with a wrong signature.");
      exit(1);
    }
    if (existingMethod) {
      continue;
    }
    MethodSignature* copySignature = methodSignature->createCopyWithIndex(methodIndex++);
    mAllMethodSignatures.push_back(copySignature);
    mNameToMethodSignatureMap[copySignature->getName()] = copySignature;
    methodIndex++;
  }
  return methodIndex;
}

vector<MethodSignature*> Interface::getAllMethodSignatures() const {
  return mAllMethodSignatures;
}

vector<Interface*> Interface::getParentInterfaces() const {
  return mParentInterfaces;
}

bool Interface::doesExtendInterface(Interface* interface) const {
  for (Interface* parentInterface : mParentInterfaces) {
    if (parentInterface == interface || parentInterface->doesExtendInterface(interface)) {
      return true;
    }
  }
  return false;
}

string Interface::getObjectNameGlobalVariableName() const {
  return "interface." + mName + ".name";
}

MethodSignature* Interface::findMethod(std::string methodName) const {
  if (!mNameToMethodSignatureMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodSignatureMap.at(methodName);
}

vector<vector<Constant*>> Interface::generateMapFunctionsIR(IRGenerationContext& context,
                                                            Model* model,
                                                            map<string, Function*>& methodFunctionMap,
                                                            GlobalVariable* typeTable,
                                                            unsigned long interfaceIndex) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Constant*> vTableArrayProtion;
  if (interfaceIndex == 0) {
    vTableArrayProtion.push_back(ConstantExpr::getNullValue(pointerType));
    Constant* bitCast = ConstantExpr::getBitCast(typeTable, pointerType);
    vTableArrayProtion.push_back(bitCast);
  } else {
    long unthunkBy = -Environment::getAddressSizeInBytes() * ((long) interfaceIndex);
    ConstantInt* unthunk = ConstantInt::get(Type::getInt64Ty(llvmContext), unthunkBy);
    vTableArrayProtion.push_back(ConstantExpr::getIntToPtr(unthunk, pointerType));
    vTableArrayProtion.push_back(ConstantExpr::getNullValue(pointerType));
  }
  for (MethodSignature* methodSignature : mAllMethodSignatures) {
    Function* modelFunction = methodFunctionMap.count(methodSignature->getName())
      ? methodFunctionMap.at(methodSignature->getName()) : NULL;
    Function* function = generateMapFunctionForMethod(context,
                                                      model,
                                                      modelFunction,
                                                      interfaceIndex,
                                                      methodSignature);
    Constant* bitCast = ConstantExpr::getBitCast(function, pointerType);
    vTableArrayProtion.push_back(bitCast);
  }
  vector<vector<Constant*>> vSubTable;
  vSubTable.push_back(vTableArrayProtion);
  for (Interface* parentInterface : mParentInterfaces) {
    vector<vector<Constant*>> parentInterfaceTables =
      parentInterface->generateMapFunctionsIR(context,
                                              model,
                                              methodFunctionMap,
                                              typeTable,
                                              interfaceIndex + vSubTable.size());
    for (vector<Constant*> parentInterfaceTable : parentInterfaceTables) {
      vSubTable.push_back(parentInterfaceTable);
    }
  }
  return vSubTable;
}

Function* Interface::generateMapFunctionForMethod(IRGenerationContext& context,
                                                  Model* model,
                                                  llvm::Function* modelFunction,
                                                  unsigned long interfaceIndex,
                                                  MethodSignature* methodSignature) const {
  Method* modelMethod = model->findMethod(methodSignature->getName());
  if (modelMethod == NULL) {
    Log::e("Method '" + methodSignature->getName() + "' of interface '" + mName +
           "' is not implemented by model '" + model->getName() + "'");
    exit(1);
  }
  
  if (modelMethod->getReturnType() != methodSignature->getReturnType()) {
    Log::e("Method '" + methodSignature->getName() + "' of interface '" + mName +
           "' has different return type when implmeneted by model '"
           + model->getName() + "'");
    exit(1);
  }
  
  if (!IMethodDescriptor::compare(modelMethod, methodSignature)) {
    Log::e("Method '" + methodSignature->getName() + "' of interface '" + mName +
           "' has different argument types when implmeneted by model '"
           + model->getName() + "'");
    exit(1);
  }

  if (interfaceIndex == 0) {
    return modelFunction;
  }
  
  string functionName =
    MethodCall::translateInterfaceMethodToLLVMFunctionName(model, this, methodSignature->getName());
  Function* function = Function::Create(modelFunction->getFunctionType(),
                                        GlobalValue::InternalLinkage,
                                        functionName,
                                        context.getModule());
  Function::arg_iterator arguments = function->arg_begin();
  llvm::Argument *argument = &*arguments;
  argument->setName("this");
  arguments++;
  vector<MethodArgument*> methodArguments = methodSignature->getArguments();
  for (MethodArgument* methodArgument : methodSignature->getArguments()) {
    llvm::Argument *argument = &*arguments;
    argument->setName(methodArgument->getName());
    arguments++;
  }
  
  generateMapFunctionBody(context, model, modelFunction, function, interfaceIndex, methodSignature);

  return function;
}

void Interface::generateMapFunctionBody(IRGenerationContext& context,
                                        Model* model,
                                        Function* modelFunction,
                                        Function* mapFunction,
                                        unsigned long interfaceIndex,
                                        MethodSignature* methodSignature) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock *basicBlock = BasicBlock::Create(llvmContext, "entry", mapFunction, 0);
  
  Function::arg_iterator arguments = mapFunction->arg_begin();
  Value* interfaceThis = storeArgumentValue(context, basicBlock, "this", model, &*arguments);
  arguments++;
  vector<Value*> argumentPointers;
  for (MethodArgument* methodArgument : methodSignature->getArguments()) {
    Value* argumentPointer = storeArgumentValue(context,
                                                basicBlock,
                                                methodArgument->getName(),
                                                methodArgument->getType(),
                                                &*arguments);
    argumentPointers.push_back(argumentPointer);
    arguments++;
  }
  
  Value* interfaceThisLoaded = new LoadInst(interfaceThis, "this", basicBlock);
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo();
  Value* castedInterfaceThis = new BitCastInst(interfaceThisLoaded, pointerType, "", basicBlock);
  Value *Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext),
                            -interfaceIndex * Environment::getAddressSizeInBytes());
  Value* modelThis = GetElementPtrInst::Create(int8Type, castedInterfaceThis, Idx, "", basicBlock);
  Value* castedModelThis = new BitCastInst(modelThis,
                                           interfaceThisLoaded->getType(),
                                           "",
                                           basicBlock);
  vector<Value*> callArguments;
  callArguments.push_back(castedModelThis);
  for (Value* argumentPointer : argumentPointers) {
    Value* loadedCallArgument = new LoadInst(argumentPointer, "", basicBlock);
    callArguments.push_back(loadedCallArgument);
  }
  
  if (modelFunction->getReturnType()->isVoidTy()) {
    CallInst::Create(modelFunction, callArguments, "", basicBlock);
    ReturnInst::Create(llvmContext, NULL, basicBlock);
  } else {
    Value* result = CallInst::Create(modelFunction, callArguments, "call", basicBlock);
    ReturnInst::Create(llvmContext, result, basicBlock);
  }
}

llvm::Value* Interface::storeArgumentValue(IRGenerationContext& context,
                                           BasicBlock* basicBlock,
                                           string variableName,
                                           IType* variableType,
                                           Value* variableValue) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  string newName = variableName + ".param";
  AllocaInst *alloc = new AllocaInst(variableType->getLLVMType(llvmContext), newName, basicBlock);
  new StoreInst(variableValue, alloc, basicBlock);
  return alloc;
}

string Interface::getName() const {
  return mName;
}

llvm::Type* Interface::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType->getPointerTo();
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

Value* Interface::getOriginalObject(IRGenerationContext& context, Value* value) {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* basicBlock = context.getBasicBlock();

  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer = new BitCastInst(value, pointerType, "", basicBlock);
  LoadInst* vTable = new LoadInst(vTablePointer, "vtable", basicBlock);
  Value *Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 0);
  GetElementPtrInst* unthunkPointer = GetElementPtrInst::Create(int8Type->getPointerTo(),
                                                                vTable,
                                                                Idx,
                                                                "unthungentry",
                                                                basicBlock);

  LoadInst* pointerToVal = new LoadInst(unthunkPointer, "unthunkbypointer", basicBlock);
  Value* unthunkBy = new PtrToIntInst(pointerToVal,
                                      Type::getInt64Ty(llvmContext),
                                      "unthunkby",
                                      basicBlock);

  BitCastInst* bitcast = new BitCastInst(value, int8Type->getPointerTo(), "", basicBlock);
  Idx[0] = unthunkBy;
  return GetElementPtrInst::Create(int8Type, bitcast, Idx, "this.ptr", basicBlock);
}
