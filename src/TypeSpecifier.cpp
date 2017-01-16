//
//  TypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/TypeSpecifier.hpp"

using namespace yazyk;

PrimitiveType TypeSpecifier::getType() const {
  return mType;
}
