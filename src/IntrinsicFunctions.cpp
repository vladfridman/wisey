//
//  IntrinsicFunctions.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/TypeBuilder.h>

#include "wisey/Environment.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/Names.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* IntrinsicFunctions::getThrowFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *functionType = TypeBuilder<void (types::i<8>*, types::i<8>*, types::i<8>*), false>
  ::get(llvmContext);
  
  AttributeSet attributeSet = AttributeSet().addAttribute(llvmContext, 1U, Attribute::NoAlias);
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("__cxa_throw", functionType, attributeSet));
}

Function* IntrinsicFunctions::getAllocateExceptionFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *functionType = TypeBuilder<types::i<8>* (types::i<64>), false>::get(llvmContext);
  
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("__cxa_allocate_exception", functionType));
}

Function* IntrinsicFunctions::getMemCopyFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *functionType =
  TypeBuilder<void (types::i<8>*, types::i<8>*, types::i<64>, types::i<32>, types::i<1>), false>
  ::get(llvmContext);
  
  AttributeSet attributeSet = AttributeSet().addAttribute(llvmContext, 1U, Attribute::NoAlias);
  return cast<Function>(context.getModule()->getOrInsertFunction("llvm.memcpy.p0i8.p0i8.i64",
                                                                 functionType,
                                                                 attributeSet));
}

Function* IntrinsicFunctions::getMemSetFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *functionType =
  TypeBuilder<void (types::i<8>*, types::i<8>, types::i<64>, types::i<32>, types::i<1>), false>
  ::get(llvmContext);
  
  AttributeSet attributeSet = AttributeSet().addAttribute(llvmContext, 1U, Attribute::NoAlias);
  return cast<Function>(context.getModule()->getOrInsertFunction("llvm.memset.p0i8.i64",
                                                                 functionType,
                                                                 attributeSet));
}

Instruction* IntrinsicFunctions::setMemoryToZero(IRGenerationContext& context,
                                                 Value* memoryPointer,
                                                 Value* size) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* bitcast = IRWriter::newBitCastInst(context,
                                            memoryPointer,
                                            Type::getInt8Ty(llvmContext)->getPointerTo());
  ConstantInt* zero = ConstantInt::get(Type::getInt8Ty(llvmContext), 0);
  
  vector<Value*> arguments;
  unsigned int memoryAlignment = Environment::getDefaultMemoryAllignment();
  arguments.push_back(bitcast);
  arguments.push_back(zero);
  arguments.push_back(size);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), memoryAlignment));
  arguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  Function* memSetFunction = getMemSetFunction(context);
  
  return IRWriter::createCallInst(context, memSetFunction, arguments, "");
}

Function* IntrinsicFunctions::getPersonalityFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *typeIdType = TypeBuilder<types::i<32> (...), false>::get(llvmContext);
  
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("__gxx_personality_v0", typeIdType));
}

Function* IntrinsicFunctions::getTypeIdFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *typeIdType = TypeBuilder<types::i<32> (types::i<8>*), false>::get(llvmContext);
  
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("llvm.eh.typeid.for", typeIdType));
}

Function* IntrinsicFunctions::getUnexpectedFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *typeIdType = TypeBuilder<void (types::i<8>*), false>::get(llvmContext);
  
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("__cxa_call_unexpected", typeIdType));
}

Function* IntrinsicFunctions::getBeginCatchFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *typeIdType = TypeBuilder<types::i<8>* (types::i<8>*), false>::get(llvmContext);
  
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("__cxa_begin_catch", typeIdType));
}

Function* IntrinsicFunctions::getEndCatchFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *typeIdType = TypeBuilder<void (), false>::get(llvmContext);
  
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("__cxa_end_catch", typeIdType));
}

Function* IntrinsicFunctions::getPrintfFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *printfType = TypeBuilder<int(char *, ...), false>::get(llvmContext);
  
  AttributeSet attributeSet = AttributeSet().addAttribute(llvmContext, 1U, Attribute::NoAlias);
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("printf", printfType, attributeSet));
}

Function* IntrinsicFunctions::getFprintfFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  GlobalVariable* stderrPointer = context.getModule()->getNamedGlobal(Names::getStdErrName());
  vector<Type*> argumentTypes;
  argumentTypes.push_back(stderrPointer->getType()->getPointerElementType());
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType *printfType = FunctionType::get(Type::getInt32Ty(llvmContext), argTypesArray, true);
  
  AttributeSet attributeSet = AttributeSet().addAttribute(llvmContext, 1U, Attribute::NoAlias);
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("fprintf", printfType, attributeSet));
}

Function* IntrinsicFunctions::getExitFunction(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *typeIdType = TypeBuilder<void (int), false>::get(llvmContext);
  
  return cast<Function>(context.getModule()->
                        getOrInsertFunction("exit", typeIdType));
}
