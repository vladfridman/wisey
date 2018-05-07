//
//  ObjectKindGlobal.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ObjectKindGlobal.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

llvm::Constant* ObjectKindGlobal::getController(IRGenerationContext& context) {
  return getName(context, "controller");
}

llvm::Constant* ObjectKindGlobal::getModel(IRGenerationContext& context) {
  return getName(context, "model");
}

llvm::Constant* ObjectKindGlobal::getNode(IRGenerationContext& context) {
  return getName(context, "node");
}

llvm::Constant* ObjectKindGlobal::getName(IRGenerationContext& context, string name) {
  LLVMContext& llvmContext = context.getLLVMContext();
  GlobalVariable* global = context.getModule()->getNamedGlobal(name);
  if (!global) {
    llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, name);
    global = new GlobalVariable(*context.getModule(),
                                stringConstant->getType(),
                                true,
                                GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                                stringConstant,
                                name);
  }
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = global->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, global, Idx);
}
