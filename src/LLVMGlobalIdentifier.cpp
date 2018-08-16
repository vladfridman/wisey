//
//  LLVMGlobal.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMGlobalIdentifier.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LLVMGlobalIdentifier::LLVMGlobalIdentifier(string name, int line) : mName(name), mLine(line) {
}

LLVMGlobalIdentifier::~LLVMGlobalIdentifier() {
}

int LLVMGlobalIdentifier::getLine() const {
  return mLine;
}

Value* LLVMGlobalIdentifier::generateIR(IRGenerationContext& context,
                                        const IType* assignToType) const {
  GlobalVariable* variablePointer = context.getModule()->getNamedGlobal(mName);
  if (!variablePointer) {
    reportError(context);
  }
  return IRWriter::newLoadInst(context, variablePointer, "");
}

const IType* LLVMGlobalIdentifier::getType(IRGenerationContext& context) const {
  const IType* type = context.getLLVMGlobalVariableType(mName);
  if (!type) {
    reportError(context);
  }
  return type;
}

bool LLVMGlobalIdentifier::isConstant() const {
  return true;
}

bool LLVMGlobalIdentifier::isAssignable() const {
  return false;
}

void LLVMGlobalIdentifier::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "::llvm::global::" << mName;
}

void LLVMGlobalIdentifier::reportError(IRGenerationContext& context) const {
  context.reportError(mLine, "LLVM global '" + mName + "' is not defined");
  throw 1;
}
