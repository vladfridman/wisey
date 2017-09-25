//
//  IObjectTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

void IObjectTypeSpecifier::printPackageToStream(iostream& stream, vector<string> package) {
  for (string packagePart : package) {
    stream << packagePart;
    stream << ".";
  }
}
