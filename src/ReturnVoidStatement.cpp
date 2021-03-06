//
//  ReturnVoidStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "Composer.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "PrimitiveTypes.hpp"
#include "ReturnVoidStatement.hpp"

using namespace llvm;
using namespace wisey;

ReturnVoidStatement::ReturnVoidStatement(int line) : mLine(line) { }

ReturnVoidStatement::~ReturnVoidStatement() { }

void ReturnVoidStatement::generateIR(IRGenerationContext& context) const {
  checkUnreachable(context, mLine);

  const IType* returnType = context.getScopes().getReturnType();
  if (returnType == NULL) {
    context.reportError(mLine, "No corresponding method found for RETURN");
    throw 1;
  }
  if (returnType != PrimitiveTypes::VOID) {
    context.reportError(mLine, "Must return a value of type " + returnType->getTypeName());
    throw 1;
  }

  Composer::setLineNumber(context, mLine);
  context.getScopes().freeOwnedMemory(context, NULL, mLine);
  if (context.getScopes().hasOwnerVariables()) {
    Composer::setLineNumber(context, mLine);
  }
  IRWriter::createReturnInst(context, NULL);
}
