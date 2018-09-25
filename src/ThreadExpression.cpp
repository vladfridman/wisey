//
//  ThreadExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string ThreadExpression::THREAD = "thread";

string ThreadExpression::CALL_STACK = "callstack";

