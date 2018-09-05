//
//  LLVMImmutablePointerOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMImmutablePointerOwnerTypeSpecifier.hpp"
#include "wisey/LLVMPointerOwnerTypeSpecifier.hpp"
#include "wisey/LLVMImmutablePointerOwnerType.hpp"
#include "wisey/LLVMImmutablePointerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMImmutablePointerOwnerTypeSpecifier::
LLVMImmutablePointerOwnerTypeSpecifier(const LLVMImmutablePointerTypeSpecifier*
                                       immutablePointerTypeSpecifier) :
mImmutablePointerTypeSpecifier(immutablePointerTypeSpecifier) {
}

LLVMImmutablePointerOwnerTypeSpecifier::~LLVMImmutablePointerOwnerTypeSpecifier() {
  delete mImmutablePointerTypeSpecifier;
}

const LLVMImmutablePointerOwnerType*
LLVMImmutablePointerOwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mImmutablePointerTypeSpecifier->getType(context)->getOwner();
}

void LLVMImmutablePointerOwnerTypeSpecifier::printToStream(IRGenerationContext& context,
                                                  std::iostream& stream) const {
  mImmutablePointerTypeSpecifier->printToStream(context, stream);
  stream << "*";
}

int LLVMImmutablePointerOwnerTypeSpecifier::getLine() const {
  return mImmutablePointerTypeSpecifier->getLine();
}
