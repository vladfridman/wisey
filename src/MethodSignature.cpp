//
//  MethodSignature.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/MethodSignature.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Method* MethodSignature::getMethod(IRGenerationContext& context) const {
  vector<MethodArgument*> arguments;
  
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    IType* type = (**iterator).getTypeSpecifier().getType(context);
    string name = (**iterator).getId().getName();
    MethodArgument* methodArgument = new MethodArgument(type, name);
    arguments.push_back(methodArgument);
  }
  
  IType* returnType = mReturnTypeSpecifier.getType(context);
  
  return new Method(mMethodName, returnType, arguments);
}
