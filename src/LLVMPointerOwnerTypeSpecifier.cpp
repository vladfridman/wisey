//
//  LLVMPointerOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerOwnerType.hpp"
#include "wisey/LLVMPointerOwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMPointerOwnerTypeSpecifier::LLVMPointerOwnerTypeSpecifier(const LLVMPointerTypeSpecifier*
                                                             baseTypeSpecifier) {
  mBaseTypeSpecifier = baseTypeSpecifier;
}

LLVMPointerOwnerTypeSpecifier::~LLVMPointerOwnerTypeSpecifier() {
  delete mBaseTypeSpecifier;
}

const LLVMPointerOwnerType* LLVMPointerOwnerTypeSpecifier::getType(IRGenerationContext&
                                                                   context) const {
  return mBaseTypeSpecifier->getType(context)->getOwner();
}

void LLVMPointerOwnerTypeSpecifier::printToStream(IRGenerationContext& context,
                                                  std::iostream& stream) const {
  stream << mBaseTypeSpecifier->getType(context)->getTypeName() << "*";
}
