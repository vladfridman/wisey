//
//  Environment.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Environment.hpp"

using namespace yazyk;

unsigned short Environment::getAddressSizeInBytes() {
  return 8;
}

unsigned int Environment::getDefaultMemoryAllignment() {
  return 4;
}
