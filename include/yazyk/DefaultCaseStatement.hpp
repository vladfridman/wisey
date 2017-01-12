//
//  DefaultCaseStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DefaultCaseStatement_h
#define DefaultCaseStatement_h

#include <llvm/IR/Constants.h>

#include "yazyk/Block.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {

/**
 * Represents a default CASE in a SWITCH statement
 */
class DefaultCaseStatement : public IStatement {
  
  Block& mBlock;
  
public:
  
  DefaultCaseStatement(Block& block) : mBlock(block) { }
  
  ~DefaultCaseStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* DefaultCaseStatement_h */
