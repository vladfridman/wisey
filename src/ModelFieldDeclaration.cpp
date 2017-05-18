//
//  ModelFieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ModelFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelFieldDeclaration::~ModelFieldDeclaration() {
  delete mTypeSpecifier;
}

ITypeSpecifier* ModelFieldDeclaration::getTypeSpecifier() {
  return mTypeSpecifier;
}

string ModelFieldDeclaration::getName() {
  return mName;
}
