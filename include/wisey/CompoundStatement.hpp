//
//  CompoundStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CompoundStatement_h
#define CompoundStatement_h

#include "wisey/Block.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

namespace yazyk {

/**
 * Wraps a block in a statement
 */
class CompoundStatement : public IStatement {
  
  Block mBlock;
  
public:
  
  CompoundStatement(Block& block) : mBlock(block) { }
  
  ~CompoundStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* CompoundStatement_h */
