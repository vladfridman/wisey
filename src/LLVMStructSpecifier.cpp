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

LLVMStructSpecifier::LLVMStructSpecifier(string name, int line) : mName(name), mLine(line) {
}

LLVMStructSpecifier::~LLVMStructSpecifier() {
}

const ILLVMType* LLVMStructSpecifier::getType(IRGenerationContext& context) const {
  return context.getLLVMStructType(mName, mLine);
}

void LLVMStructSpecifier::printToStream(IRGenerationContext& context,
                                        iostream& stream) const {
  stream << context.getLLVMStructType(mName, mLine)->getTypeName();
}

int LLVMStructSpecifier::getLine() const {
  return mLine;
}
