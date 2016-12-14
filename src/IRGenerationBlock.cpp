//
//  IRGenerationBlock.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/IRGenerationBlock.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

BasicBlock* IRGenerationBlock::getBlock() {
  return mBlock;
}

void IRGenerationBlock::setBlock(BasicBlock* block) {
  mBlock = block;
}

map<string, Value*>& IRGenerationBlock::getLocals() {
  return mLocals;
}
