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

ReturnVoidStatement::ReturnVoidStatement(int line) : mLine(line) { }

ReturnVoidStatement::~ReturnVoidStatement() { }

void ReturnVoidStatement::generateIR(IRGenerationContext& context) const {
  context.getScopes().freeOwnedMemory(context, mLine);

  IRWriter::createReturnInst(context, NULL);
}
