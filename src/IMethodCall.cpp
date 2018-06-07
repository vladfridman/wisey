//
//  IMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Composer.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string IMethodCall::translateObjectMethodToLLVMFunctionName(const IObjectType* object,
                                                            string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return object->getTypeName() + "." + methodName;
}
