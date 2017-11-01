//
//  IObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

llvm::Constant* IObjectType::getObjectNamePointer(const IObjectType *object,
                                                  IRGenerationContext& context) {
  GlobalVariable* nameGlobal =
    context.getModule()->getNamedGlobal(object->getObjectNameGlobalVariableName());
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = nameGlobal->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
}

Value* IObjectType::getReferenceCounterPointer(IRGenerationContext& context, Value* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* type = Type::getInt64Ty(llvmContext)->getPointerTo()->getPointerTo();
  Value* genericPointer = IRWriter::newBitCastInst(context, object, type);
  return IRWriter::newLoadInst(context, genericPointer, "refCounterPointer");
}

void IObjectType::incrementReferenceCounterForObject(IRGenerationContext& context, Value* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* counterPointer = getReferenceCounterPointer(context, object);
  Value* value = IRWriter::newLoadInst(context, counterPointer, "refCounter");
  
  llvm::Constant* one = llvm::ConstantInt::get(Type::getInt64Ty(llvmContext), 1);
  Value* addition = IRWriter::createBinaryOperator(context, Instruction::Add, value, one, "");
  IRWriter::newStoreInst(context, addition, counterPointer);
}

void IObjectType::decrementReferenceCounterForObject(IRGenerationContext& context, Value* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* counterPointer = getReferenceCounterPointer(context, object);
  Value* value = IRWriter::newLoadInst(context, counterPointer, "refCounter");
  
  llvm::Constant* one = llvm::ConstantInt::get(Type::getInt64Ty(llvmContext), 1);
  Value* addition = IRWriter::createBinaryOperator(context, Instruction::Sub, value, one, "");
  IRWriter::newStoreInst(context, addition, counterPointer);
}


Value* IObjectType::getReferenceCountForObject(IRGenerationContext& context, Value* object) {
  Value* counterPointer = getReferenceCounterPointer(context, object);
  return IRWriter::newLoadInst(context, counterPointer, "refCounter");
}
