//
//  ProgramPrefix.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramPrefix_h
#define ProgramPrefix_h

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Things that need to be run before the parsed program code
 */
class ProgramPrefix : public IStatement {

public:
  
  void prototype(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* ProgramPrefix_h */
