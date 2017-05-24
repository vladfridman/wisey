//
//  ReturnVoidStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ReturnVoidStatement.hpp"

#include <llvm/IR/Instructions.h>

using namespace llvm;
using namespace wisey;

void ReturnVoidStatement::prototype(IRGenerationContext& context) const {
}

Value* ReturnVoidStatement::generateIR(IRGenerationContext& context) const {
  return IRWriter::createReturnInst(context, NULL);
}
