//
//  IVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IVariable.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

string IVariable::getTemporaryVariableName(const void* object) {
  ostringstream stream;
  stream << "__tmp" << (long) object;
  return stream.str();
}

IVariable* IVariable::getVariable(IRGenerationContext& context, std::string name, int line) {
  IVariable* variable = context.getScopes().getVariable(name);
  if (variable != NULL) {
    return variable;
  }
  
  context.reportError(line, "Undeclared variable '" + name + "'");
  exit(1);
}
