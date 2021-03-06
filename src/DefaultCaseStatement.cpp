//
//  DefaultCaseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "DefaultCaseStatement.hpp"

using namespace llvm;
using namespace wisey;

DefaultCaseStatement::DefaultCaseStatement(Block* block) : mBlock(block) { }

DefaultCaseStatement::~DefaultCaseStatement() {
  delete mBlock;
}

void DefaultCaseStatement::generateIR(IRGenerationContext& context) const {
  mBlock->generateIR(context);
}

