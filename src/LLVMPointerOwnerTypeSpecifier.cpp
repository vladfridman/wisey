//
//  LLVMPointerOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerOwnerTypeSpecifier.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMPointerOwnerTypeSpecifier::
LLVMPointerOwnerTypeSpecifier(const LLVMPointerTypeSpecifier* pointerTypeSpecifier) :
mPointerTypeSpecifier(pointerTypeSpecifier) {
}

LLVMPointerOwnerTypeSpecifier::~LLVMPointerOwnerTypeSpecifier() {
  delete mPointerTypeSpecifier;
}

const LLVMPointerOwnerType* LLVMPointerOwnerTypeSpecifier::getType(IRGenerationContext&
                                                                   context) const {
  return mPointerTypeSpecifier->getType(context)->getOwner();
}

void LLVMPointerOwnerTypeSpecifier::printToStream(IRGenerationContext& context,
                                                  std::iostream& stream) const {
  mPointerTypeSpecifier->printToStream(context, stream);
  stream << "*";
}
