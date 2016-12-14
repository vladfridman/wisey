//
//  ReturnVoidStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ReturnVoidStatement.hpp"

#include <llvm/IR/Instructions.h>

using namespace llvm;
using namespace yazyk;

Value* ReturnVoidStatement::generateIR(IRGenerationContext& context) {
  return ReturnInst::Create(context.getLLVMContext(), NULL, context.currentBlock());
}
