//
//  ProgramFile.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ProgramFile::~ProgramFile() {
  delete mBlock;
}

void ProgramFile::prototype(IRGenerationContext& context) const {
}

Value* ProgramFile::generateIR(IRGenerationContext& context) const {
  context.setPackage(mPackage);
  
  return mBlock->generateIR(context);
}
