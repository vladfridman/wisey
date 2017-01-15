//
//  BreakStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/25/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef BreakStatement_h
#define BreakStatement_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents a break statement used inside loops and switch statements
 */
class BreakStatement : public IStatement {
  
public:
  BreakStatement() { }
  
  ~BreakStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
}

#endif /* BreakStatement_h */