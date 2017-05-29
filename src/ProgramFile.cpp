//
//  ProgramFile.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ProgramFile::~ProgramFile() {
  delete mProgramBlock;
}

void ProgramFile::prototypeObjects(IRGenerationContext& context) const {
  context.setPackage(mPackage);

  mProgramBlock->prototypeObjects(context);
}

Value* ProgramFile::generateIR(IRGenerationContext& context) const {
  context.setPackage(mPackage);
  
  return mProgramBlock->generateIR(context);
}
