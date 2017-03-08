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
#include "yazyk/ModelMethodCall.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Interface::~Interface() {
  mMethods.clear();
}

Interface::Interface(string name, StructType* structType, vector<Method*> methods) {
  mName = name;
  mStructType = structType;
  mMethods = methods;
  for (Method* method : mMethods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

Method* Interface::findMethod(std::string methodName) const {
  if (!mNameToMethodMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodMap.at(methodName);
}

vector<Constant*> Interface::generateMapFunctionsIR(IRGenerationContext& context,
                                                    Model* model,
                                                    map<string, Function*>& methodFunctionMap,
                                                    int interfaceIndex) const {
  Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<Constant*> vTableArrayProtion;
  for (Method* method : mMethods) {
    Function* modelFunction = methodFunctionMap.count(method->getName())
      ? methodFunctionMap.at(method->getName()) : NULL;
    Function* function = generateMapFunctionForMethod(context,
                                                      model,
                                                      modelFunction,
                                                      interfaceIndex,
                                                      method);
    Constant* bitCast = ConstantExpr::getBitCast(function, pointerType);
    vTableArrayProtion.push_back(bitCast);
  }
  return vTableArrayProtion;
}

Function* Interface::generateMapFunctionForMethod(IRGenerationContext& context,
                                                  Model* model,
                                                  llvm::Function* modelFunction,
                                                  int interfaceIndex,
                                                  Method* method) const {
  Method* modelMethod = model->findMethod(method->getName());
  if (modelMethod == NULL) {
    Log::e("Method '" + method->getName() + "' of interface '" + mName +
           "' is not implemented by model '" + model->getName() + "'");
    exit(1);
  }
  
  if (modelMethod->getReturnType() != method->getReturnType()) {
    Log::e("Method '" + method->getName() + "' of interface '" + mName +
           "' has different return type when implmeneted by model '"
           + model->getName() + "'");
    exit(1);
  }
  
  if (!modelMethod->equals(method)) {
    Log::e("Method '" + method->getName() + "' of interface '" + mName +
           "' has different argument types when implmeneted by model '"
           + model->getName() + "'");
    exit(1);
  }

  if (interfaceIndex == 0) {
    return modelFunction;
  }
  
  string functionName =
    ModelMethodCall::translateInterfaceMethodToLLVMFunctionName(model, this,method->getName());
  Function* function = Function::Create(modelFunction->getFunctionType(),
                                        GlobalValue::InternalLinkage,
                                        functionName,
                                        context.getModule());
  Function::arg_iterator arguments = function->arg_begin();
  llvm::Argument *argument = &*arguments;
  argument->setName("this");
  arguments++;
  vector<MethodArgument*> methodArguments = method->getArguments();
  for (MethodArgument* methodArgument : method->getArguments()) {
    llvm::Argument *argument = &*arguments;
    argument->setName(methodArgument->getName());
    arguments++;
  }
  
  generateMapFunctionBody(context, model, modelFunction, function, interfaceIndex, method);

  return function;
}

void Interface::generateMapFunctionBody(IRGenerationContext& context,
                                        Model* model,
                                        Function* modelFunction,
                                        Function* mapFunction,
                                        int interfaceIndex,
                                        Method* method) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock *basicBlock = BasicBlock::Create(llvmContext, "entry", mapFunction, 0);
  
  Function::arg_iterator arguments = mapFunction->arg_begin();
  Value* interfaceThis = storeArgumentValue(context, basicBlock, "this", model, &*arguments);
  arguments++;
  vector<Value*> argumentPointers;
  for (MethodArgument* methodArgument : method->getArguments()) {
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
