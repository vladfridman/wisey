//
//  ProgramFile.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ProgramFile::~ProgramFile() {
  delete mBlock;
}

Value* ProgramFile::generateIR(IRGenerationContext& context) const {
  return mBlock->generateIR(context);
}
