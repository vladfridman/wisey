//
//  BooleanConstants.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/BooleanConstant.hpp"
#include "wisey/BooleanConstants.hpp"

using namespace yazyk;

BooleanConstant* BooleanConstants::BOOLEAN_TRUE = new BooleanConstant(true);

BooleanConstant* BooleanConstants::BOOLEAN_FALSE = new BooleanConstant(false);
