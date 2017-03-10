//
//  MethodSignatureDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/MethodSignatureDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Method* MethodSignatureDeclaration::createMethod(IRGenerationContext& context, unsigned long index) const {
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
  
  return new Method(mMethodName, returnType, arguments, index, NULL);
}
