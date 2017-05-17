//
//  Environment.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Environment.hpp"

using namespace wisey;

unsigned short Environment::getAddressSizeInBytes() {
  return 8;
}

unsigned int Environment::getDefaultMemoryAllignment() {
  return 4;
}
