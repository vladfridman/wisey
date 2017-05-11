//
//  IntrinsicFunctions.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/TypeBuilder.h>

#include "yazyk/IntrinsicFunctions.hpp"

using namespace llvm;
using namespace yazyk;

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