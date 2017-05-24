//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Things that need to be run after the parsed program code
 */
class ProgramSuffix : public IStatement {
    
public:
  
  void prototype(IRGenerationContext& context) const override;
 
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

