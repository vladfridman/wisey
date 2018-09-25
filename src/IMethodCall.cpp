//
//  IMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "Composer.hpp"
#include "IMethodCall.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string IMethodCall::translateObjectMethodToLLVMFunctionName(const IObjectType* object,
                                                            string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return object->getTypeName() + ".method." + methodName;
}
