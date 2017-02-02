//
//  ModelFieldDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ModelFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ITypeSpecifier& ModelFieldDeclaration::getType() {
  return mType;
}

string ModelFieldDeclaration::getName() {
  return mName;
}
