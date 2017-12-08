//
//  IObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Composer.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

llvm::Constant* IObjectType::getObjectNamePointer(const IObjectType* object,
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
  assert(object->getType()->isPointerTy());
  assert(!object->getType()->getPointerElementType()->isPointerTy());
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* type = Type::getInt64Ty(llvmContext)->getPointerTo();
  return IRWriter::newBitCastInst(context, object, type);
}

Value* IObjectType::getReferenceCountForObject(IRGenerationContext& context, Value* object) {
  Value* counterPointer = getReferenceCounterPointer(context, object);
  return IRWriter::newLoadInst(context, counterPointer, "refCounter");
}

bool IObjectType::checkAccess(const IObjectType* from, const IObjectType* to) {
  if (to->getAccessLevel() == AccessLevel::PUBLIC_ACCESS) {
    return true;
  }
  
  if (from == to) {
    return true;
  }
  
  if (from->getInnerObject(to->getShortName()) == to) {
    return true;
  }
  
  Log::e("Object " + to->getName() + " is not accessable from object " + from->getName());
  exit(1);
}
