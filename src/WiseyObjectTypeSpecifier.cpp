//
//  WiseyObjectTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "WiseyObjectType.hpp"
#include "WiseyObjectTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyObjectTypeSpecifier::WiseyObjectTypeSpecifier(int line) : mLine(line) {
}

WiseyObjectTypeSpecifier::~WiseyObjectTypeSpecifier() {
}

const WiseyObjectType* WiseyObjectTypeSpecifier::getType(IRGenerationContext& context) const {
  return WiseyObjectType::WISEY_OBJECT_TYPE;
}

void WiseyObjectTypeSpecifier::printToStream(IRGenerationContext& context,
                                            std::iostream& stream) const {
  stream << "::wisey::object";
}

int WiseyObjectTypeSpecifier::getLine() const {
  return mLine;
}
