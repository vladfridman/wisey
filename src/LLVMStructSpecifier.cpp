//
//  LLVMStructSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMStructSpecifier.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

LLVMStructSpecifier::LLVMStructSpecifier(string name) : mName(name) {
}

LLVMStructSpecifier::~LLVMStructSpecifier() {
}

const ILLVMType* LLVMStructSpecifier::getType(IRGenerationContext& context) const {
  return context.getLLVMStructType(mName);
}

void LLVMStructSpecifier::printToStream(IRGenerationContext& context,
                                        iostream& stream) const {
  stream << "::llvm::struct::" << context.getLLVMStructType(mName)->getTypeName();
}
