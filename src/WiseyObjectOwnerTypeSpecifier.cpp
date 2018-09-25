//
//  WiseyObjectOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "WiseyObjectOwnerType.hpp"
#include "WiseyObjectOwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyObjectOwnerTypeSpecifier::WiseyObjectOwnerTypeSpecifier(int line) : mLine(line) {
}

WiseyObjectOwnerTypeSpecifier::~WiseyObjectOwnerTypeSpecifier() {
}

const WiseyObjectOwnerType* WiseyObjectOwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE;
}

void WiseyObjectOwnerTypeSpecifier::printToStream(IRGenerationContext& context,
                                                 std::iostream& stream) const {
  stream << "::wisey::object*";
}

int WiseyObjectOwnerTypeSpecifier::getLine() const {
  return mLine;
}
