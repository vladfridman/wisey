//
//  EmptyStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/EmptyStatement.hpp"

using namespace llvm;
using namespace wisey;

const EmptyStatement EmptyStatement::EMPTY_STATEMENT = EmptyStatement();

Value* EmptyStatement::generateIR(IRGenerationContext& context) const {
  return NULL;
}
