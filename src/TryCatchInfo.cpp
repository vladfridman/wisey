//
//  TryCatchInfo.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Block.hpp"
#include "wisey/Catch.hpp"
#include "wisey/TryCatchInfo.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TryCatchInfo::~TryCatchInfo() {
  mFinallyBlock->getStatements().clear();
  delete mFinallyBlock;
}

BasicBlock* TryCatchInfo::getLandingPadBlock() {
  return mLandingPadBlock;
}

BasicBlock* TryCatchInfo::getContinueBlock() {
  return mContinueBlock;
}

Block* TryCatchInfo::getFinallyBlock() {
  return mFinallyBlock;
}

vector<Catch*> TryCatchInfo::getCatchList() {
  return mCatchList;
}
