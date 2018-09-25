//
//  WiseyModelOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "WiseyModelOwnerType.hpp"
#include "WiseyModelOwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyModelOwnerTypeSpecifier::WiseyModelOwnerTypeSpecifier(int line) : mLine(line) {
}

WiseyModelOwnerTypeSpecifier::~WiseyModelOwnerTypeSpecifier() {
}

const WiseyModelOwnerType* WiseyModelOwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE;
}

void WiseyModelOwnerTypeSpecifier::printToStream(IRGenerationContext& context,
                                                 std::iostream& stream) const {
  stream << "::wisey::model*";
}

int WiseyModelOwnerTypeSpecifier::getLine() const {
  return mLine;
}
