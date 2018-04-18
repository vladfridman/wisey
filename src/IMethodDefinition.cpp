//
//  IMethodDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IMethodDefinition.hpp"
#include "wisey/Names.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

vector<const MethodArgument*> IMethodDefinition::
createArgumnetList(IRGenerationContext& context, VariableList argumentDefinitions) {
  vector<const MethodArgument*> arguments;
  
  for (VariableList::const_iterator iterator = argumentDefinitions.begin();
       iterator != argumentDefinitions.end();
       iterator++) {
    const IType* type = (**iterator).getTypeSpecifier()->getType(context);
    string name = (**iterator).getIdentifier()->getIdentifierName();
    MethodArgument* methodArgument = new MethodArgument(type, name);
    arguments.push_back(methodArgument);
  }
  
  return arguments;
}

vector<const Model*> IMethodDefinition::createExceptionList(IRGenerationContext& context,
                                                             vector<IModelTypeSpecifier*>
                                                             thrownExceptions) {
  vector<const Model*> exceptions;
  for (IModelTypeSpecifier* typeSpecifier : thrownExceptions) {
    exceptions.push_back(typeSpecifier->getType(context));
  }
  
  return exceptions;
}
