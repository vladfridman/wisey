//
//  IStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void IStatement::checkUnreachable(IRGenerationContext& context, int line) {
  if(context.getBasicBlock()->getTerminator()) {
    context.reportError(line, "Statement unreachable");
    throw 1;
  }
}
