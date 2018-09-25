//
//  GlobalStringConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "GlobalStringConstant.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

llvm::Constant* GlobalStringConstant::get(IRGenerationContext& context, string value) {
  LLVMContext& llvmContext = context.getLLVMContext();
  GlobalVariable* global = context.getModule()->getNamedGlobal(value);
  
  if (!global) {
    llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, value);
    global = new GlobalVariable(*context.getModule(),
                                stringConstant->getType(),
                                true,
                                GlobalValue::LinkageTypes::InternalLinkage,
                                stringConstant,
                                value);
  }
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = global->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, global, Idx);
}
