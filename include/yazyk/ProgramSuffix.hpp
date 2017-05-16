//
//  ProgramSuffix.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Things that need to be run after the parsed program code
 */
class ProgramSuffix : public IStatement {
    
public:
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace yazyk */

