//
//  IVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/IVariable.hpp"

using namespace std;
using namespace wisey;

string IVariable::getTemporaryVariableName(const void* object) {
  ostringstream stream;
  stream << "__tmp" << (long) object;
  return stream.str();
}
