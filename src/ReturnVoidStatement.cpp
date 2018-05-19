//
//  ReturnVoidStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/Composer.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ReturnVoidStatement.hpp"

using namespace llvm;
using namespace wisey;

ReturnVoidStatement::ReturnVoidStatement(int line) : mLine(line) { }

ReturnVoidStatement::~ReturnVoidStatement() { }

void ReturnVoidStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);

  Composer::setLineNumber(context, mLine);
  llvm::PointerType* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* null = ConstantPointerNull::get(int8Pointer);
  context.getScopes().freeOwnedMemory(context, null, mLine);
  Composer::popCallStack(context);
  IRWriter::createReturnInst(context, NULL);
}
