//
//  ObjectKindGlobal.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/GlobalStringConstant.hpp"
#include "wisey/ObjectKindGlobal.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

llvm::Constant* ObjectKindGlobal::getController(IRGenerationContext& context) {
  return GlobalStringConstant::get(context, "controller");
}

llvm::Constant* ObjectKindGlobal::getModel(IRGenerationContext& context) {
  return GlobalStringConstant::get(context, "model");
}

llvm::Constant* ObjectKindGlobal::getNode(IRGenerationContext& context) {
  return GlobalStringConstant::get(context, "node");
}

llvm::Constant* ObjectKindGlobal::getThread(IRGenerationContext& context) {
  return GlobalStringConstant::get(context, "thread");
}
