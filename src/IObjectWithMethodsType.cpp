//
//  IObjectWithMethodsType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/IObjectWithMethodsType.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Constant* IObjectWithMethodsType::getObjectNamePointer(const IObjectWithMethodsType *object,
                                                       IRGenerationContext& context) {
  GlobalVariable* nameGlobal =
    context.getModule()->getGlobalVariable(object->getObjectNameGlobalVariableName());
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = nameGlobal->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
}