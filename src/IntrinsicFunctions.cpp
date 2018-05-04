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

Function* IntrinsicFunctions::getPrintfFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("printf");
}

Function* IntrinsicFunctions::getFprintfFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("fprintf");
}

Function* IntrinsicFunctions::getThrowFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("__cxa_throw");
}

Function* IntrinsicFunctions::getAllocateExceptionFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("__cxa_allocate_exception");
}

Function* IntrinsicFunctions::getUnexpectedFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("__cxa_call_unexpected");
}

Function* IntrinsicFunctions::getBeginCatchFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("__cxa_begin_catch");
}

Function* IntrinsicFunctions::getEndCatchFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("__cxa_end_catch");
}

Function* IntrinsicFunctions::getPersonalityFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("__gxx_personality_v0");
}

Function* IntrinsicFunctions::getTypeIdFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("llvm.eh.typeid.for");
}

Function* IntrinsicFunctions::getMemCopyFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("llvm.memcpy.p0i8.p0i8.i64");
}

Function* IntrinsicFunctions::getMemSetFunction(IRGenerationContext& context) {
  return context.getModule()->getFunction("llvm.memset.p0i8.i64");
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
  
  return IRWriter::createCallInst(context, memSetFunction, arguments, "", 0);
}
