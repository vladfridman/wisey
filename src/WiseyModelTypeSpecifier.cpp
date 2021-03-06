//
//  WiseyModelTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "IRGenerationContext.hpp"
#include "WiseyModelType.hpp"
#include "WiseyModelTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

WiseyModelTypeSpecifier::WiseyModelTypeSpecifier(int line) : mLine(line) {
}

WiseyModelTypeSpecifier::~WiseyModelTypeSpecifier() {
}

const WiseyModelType* WiseyModelTypeSpecifier::getType(IRGenerationContext& context) const {
  return WiseyModelType::WISEY_MODEL_TYPE;
}

void WiseyModelTypeSpecifier::printToStream(IRGenerationContext& context,
                                            std::iostream& stream) const {
  stream << "::wisey::model";
}

int WiseyModelTypeSpecifier::getLine() const {
  return mLine;
}
