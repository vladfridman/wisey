//
//  WiseyObjectTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/WiseyObjectType.hpp"
#include "wisey/WiseyObjectTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyObjectTypeSpecifier::WiseyObjectTypeSpecifier() {
}

WiseyObjectTypeSpecifier::~WiseyObjectTypeSpecifier() {
}

const WiseyObjectType* WiseyObjectTypeSpecifier::getType(IRGenerationContext& context) const {
  return WiseyObjectType::LLVM_OBJECT_TYPE;
}

void WiseyObjectTypeSpecifier::printToStream(IRGenerationContext& context,
                                            std::iostream& stream) const {
  stream << "::wisey::object";
}
