//
//  BreakStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/25/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef BreakStatement_h
#define BreakStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents a break statement used inside loops and switch statements
 */
class BreakStatement : public IStatement {
  
public:
  BreakStatement() { }
  
  ~BreakStatement() { }
  
  void prototypeObjects(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
}

#endif /* BreakStatement_h */
