//
//  ThreadInfrastructure.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/ThreadInfrastructure.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

Function* ThreadInfrastructure::getThreadCreateFunction(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getThreadCreateFunctionName());
  
  return function ? function : defineThreadCreateFunction(context);
}

Function* ThreadInfrastructure::defineThreadCreateFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  PointerType* genericPointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<Type*> runnerFunctionArgumentTypes;
  runnerFunctionArgumentTypes.push_back(genericPointer);
  ArrayRef<Type*> runnerFunctionArgumentTypesArray = ArrayRef<Type*>(runnerFunctionArgumentTypes);
  FunctionType* runnerFunctionType = FunctionType::get(genericPointer,
                                                       runnerFunctionArgumentTypesArray,
                                                       false);

  vector<Type*> argumentTypes;
  argumentTypes.push_back(getNativeThreadStruct(context)->getPointerTo()->getPointerTo());
  argumentTypes.push_back(getNativeThreadAttributesStruct(context)->getPointerTo());
  argumentTypes.push_back(runnerFunctionType->getPointerTo());
  argumentTypes.push_back(genericPointer);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(ftype,
                          GlobalValue::ExternalLinkage,
                          getThreadCreateFunctionName(),
                          context.getModule());
}

string ThreadInfrastructure::getThreadCreateFunctionName() {
  return "pthread_create";
}

StructType* ThreadInfrastructure::getNativeThreadStruct(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  StructType* structType = context.getModule()->getTypeByName(getNativeThreadStructName());
  if (structType) {
    return structType;
  }

  structType = StructType::create(llvmContext, getNativeThreadStructName());

  vector<Type*> structTypes;
  structTypes.push_back(Type::getInt64Ty(llvmContext));
  structTypes.push_back(getNativeThreadHandlerStruct(context));
  structTypes.push_back(llvm::ArrayType::get(Type::getInt8Ty(llvmContext), 8176));
  structType->setBody(structTypes);
  
  return structType;
}

StructType* ThreadInfrastructure::getNativeThreadHandlerStruct(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  StructType* structType = context.getModule()->getTypeByName(getNativeThreadHandlerName());
  if (structType) {
    return structType;
  }
  
  structType = StructType::create(llvmContext, getNativeThreadHandlerName());

  PointerType* genericPointer = Type::getInt64Ty(llvmContext)->getPointerTo();
  vector<Type*> funcArgumentTypes;
  funcArgumentTypes.push_back(genericPointer);
  ArrayRef<Type*> funcArgTypesArray = ArrayRef<Type*>(funcArgumentTypes);
  FunctionType* funcType = FunctionType::get(genericPointer, funcArgTypesArray, false);

  vector<Type*> structTypes;
  structTypes.push_back(funcType->getPointerTo());
  structTypes.push_back(genericPointer);
  structTypes.push_back(structType->getPointerTo());
  structType->setBody(structTypes);
  
  return structType;
}

StructType* ThreadInfrastructure::getNativeThreadAttributesStruct(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  StructType* structType = context.getModule()->getTypeByName(getNativeThreadAttributesName());
  if (structType) {
    return structType;
  }
  
  structType = StructType::create(llvmContext, getNativeThreadAttributesName());
  
  vector<Type*> structTypes;
  structTypes.push_back(Type::getInt64Ty(llvmContext));
  structTypes.push_back(llvm::ArrayType::get(Type::getInt8Ty(llvmContext), 56));
  structType->setBody(structTypes);
  
  return structType;
}

string ThreadInfrastructure::getNativeThreadStructName() {
  return "struct._opaque_pthread_t";
}

string ThreadInfrastructure::getNativeThreadHandlerName() {
  return "struct.__darwin_pthread_handler_rec";
}

string ThreadInfrastructure::getNativeThreadAttributesName() {
  return "struct._opaque_pthread_attr_t";
}

NativeType* ThreadInfrastructure::createNativeThreadType(IRGenerationContext& context) {
  return new NativeType(getNativeThreadStruct(context)->getPointerTo());
}

NativeType* ThreadInfrastructure::createNativeThreadAttributesType(IRGenerationContext& context) {
  return new NativeType(getNativeThreadAttributesStruct(context)->getPointerTo());
}

Function* ThreadInfrastructure::getRunBridgeFunction(IRGenerationContext& context,
                                                     const Thread* thread) {
  Function* function = context.getModule()->getFunction(getRunBridgeFunctionName(thread));
  if (function) {
    return function;
  }
  
  function = defineRunBridgeFunction(context, thread);
  context.addComposingCallback1Objects(composeRunBridgeFunction, function, thread);

  return function;
}

Function* ThreadInfrastructure::defineRunBridgeFunction(IRGenerationContext& context,
                                                        const Thread* thread) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Type* genericPointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(genericPointerType);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType* ftype = FunctionType::get(genericPointerType, argTypesArray, false);
  
  string functionName = getRunBridgeFunctionName(thread);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, functionName, context.getModule());
}

void ThreadInfrastructure::composeRunBridgeFunction(IRGenerationContext& context,
                                                    Function* function,
                                                    const IObjectType* thread) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* llvmArgument = &*llvmArguments;
  llvmArgument->setName("thread");
  Value* threadArgument = llvmArgument;
  
  llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvmContext, "entry", function);
  context.setBasicBlock(entry);
  
  Type* threadLLVMType = thread->getLLVMType(context);
  Value* threadObject = IRWriter::newBitCastInst(context, threadArgument, threadLLVMType);
  
  Thread* mainThread = context.getThread(Names::getMainThreadFullName());
  Type* threadControllerLLVMType = mainThread->getLLVMType(context);
  Value* threadControllerObject = IRWriter::newBitCastInst(context,
                                                           threadArgument,
                                                           threadControllerLLVMType);
  
  string runFunctionName = IMethodCall::translateObjectMethodToLLVMFunctionName(thread, "run");
  
  Function* runFunction = context.getModule()->getFunction(runFunctionName);
  vector<Value*> callArguments;
  callArguments.push_back(threadObject);
  callArguments.push_back(threadControllerObject);
  
  IRWriter::createCallInst(context, runFunction, callArguments, "");
  
  IRWriter::createReturnInst(context, threadArgument);
}

string ThreadInfrastructure::getRunBridgeFunctionName(const Thread* thread) {
  return IMethodCall::translateObjectMethodToLLVMFunctionName(thread, "runBridge");
}
