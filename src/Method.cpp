//
//  Method.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Method.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

bool Method::equals(Method* method) const {
  vector<MethodArgument*> thatArugments = method->getArguments();
  if (mArguments.size() != thatArugments.size()) {
    return false;
  }
  
  vector<MethodArgument*>::const_iterator thatArgumentIterator = thatArugments.begin();
  for (vector<MethodArgument*>::const_iterator thisArgumentIterator = mArguments.begin();
       thisArgumentIterator != mArguments.end();
       thisArgumentIterator++, thatArgumentIterator++) {
    MethodArgument* thisArgument = *thisArgumentIterator;
    MethodArgument* thatArgument = *thatArgumentIterator;
    if (thisArgument->getType() != thatArgument->getType()) {
      return false;
    }
  }
  
  return !mName.compare(method->getName()) && mReturnType == method->getReturnType();
}
