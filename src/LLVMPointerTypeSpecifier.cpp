//
//  LLVMPointerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMPointerTypeSpecifier::LLVMPointerTypeSpecifier(const ILLVMTypeSpecifier* baseTypeSpecifier,
                                                   int line) :
mBaseTypeSpecifier(baseTypeSpecifier), mLine(line) {
}

LLVMPointerTypeSpecifier::~LLVMPointerTypeSpecifier() {
  delete mBaseTypeSpecifier;
}

const LLVMPointerType* LLVMPointerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mBaseTypeSpecifier->getType(context)->getPointerType(context, mLine);
}

void LLVMPointerTypeSpecifier::printToStream(IRGenerationContext& context,
                                             std::iostream& stream) const {
  stream << mBaseTypeSpecifier->getType(context)->getTypeName() << "::pointer";
}

int LLVMPointerTypeSpecifier::getLine() const {
  return mBaseTypeSpecifier->getLine();
}
