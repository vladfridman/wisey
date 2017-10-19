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

