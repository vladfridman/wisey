//
//  ProgramPrefix.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramPrefix_h
#define ProgramPrefix_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Things that need to be run before the parsed program code
 */
class ProgramPrefix : public IStatement {

public:
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace yazyk */

#endif /* ProgramPrefix_h */
