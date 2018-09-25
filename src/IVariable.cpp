//
//  IVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "IRGenerationContext.hpp"
#include "IVariable.hpp"
#include "Log.hpp"

using namespace std;
using namespace wisey;

string IVariable::getTemporaryVariableName(const void* object) {
  ostringstream stream;
  stream << "__tmp" << (long) object;
  return stream.str();
}

IVariable* IVariable::getVariable(IRGenerationContext& context, std::string name, int line) {
  IVariable* variable = context.getScopes().getVariable(name);
  if (variable == NULL) {
    context.reportError(line, "Undeclared variable '" + name + "'");
    throw 1;
  }
  
  if (variable->isField() && !context.getThis()) {
    context.reportError(line, "Member variables are not accessible from static methods");
    throw 1;
  }
  
  return variable;
}
