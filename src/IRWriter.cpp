//
//  IRWriter.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ReturnInst* IRWriter::createReturnInst(IRGenerationContext& context, Value* returnValue) {
  if (context.getBasicBlock()->getTerminator()) {
    return NULL;
  }
  return ReturnInst::Create(context.getLLVMContext(), returnValue, context.getBasicBlock());
}
