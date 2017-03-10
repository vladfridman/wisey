//
//  Interface.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Cast.hpp"
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
  mNameToMethodSignatureMap.clear();
}

Interface::Interface(string name,
                     StructType* structType,
                     vector<MethodSignature*> methodSignatures) {
  mName = name;
  mStructType = structType;
  mMethodSignatures = methodSignatures;
  for (MethodSignature* methodSignature : methodSignatures) {
    mNameToMethodSignatureMap[methodSignature->getName()] = methodSignature;
  }
}

MethodSignature* Interface::findMethod(std::string methodName) const {
  if (!mNameToMethodSignatureMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodSignatureMap.at(methodName);
}

vector<Constant*> Interface::generateMapFunctionsIR(IRGenerationContext& context,
                                                    Model* model,
                                                    map<string, Function*>& methodFunctionMap,
                                                    int interfaceIndex) const {
  Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<Constant*> vTableArrayProtion;
  for (MethodSignature* methodSignature : mMethodSignatures) {
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
  return vTableArrayProtion;
}

Function* Interface::generateMapFunctionForMethod(IRGenerationContext& context,
                                                  Model* model,
                                                  llvm::Function* modelFunction,
                                                  int interfaceIndex,
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
  
  if (!modelMethod->equals(methodSignature)) {
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
                                        int interfaceIndex,
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
  Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), -8 * interfaceIndex);
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
