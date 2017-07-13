//
//  TryCatchInfo.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/TryCatchInfo.hpp"

using namespace llvm;
using namespace wisey;

BasicBlock* TryCatchInfo::getLandingPadBlock() {
  return mLandingPadBlock;
}

BasicBlock* TryCatchInfo::getContinueBlock() {
  return mContinueBlock;
}

const IStatement* TryCatchInfo::getFinallyStatement() {
  return mFinallyStatement;
}
