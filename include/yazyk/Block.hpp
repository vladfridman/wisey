//
//  Block.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Block_h
#define Block_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {

/**
 * Represents a block of statements
 */
class Block : public IExpression {
  StatementList mStatements;
  
public:
  Block() { }

  ~Block() { }
  
  StatementList& getStatements();
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};
  
} /* namespace yazyk */

#endif /* Block_h */
