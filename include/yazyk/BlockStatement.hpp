//
//  BlockStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BlockStatement_h
#define BlockStatement_h

#include "yazyk/Block.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {

/**
 * Wraps a block in a statement
 */
class BlockStatement : public IStatement {
  
Block mBlock;
  
public:
  
  BlockStatement(Block& block) : mBlock(block) { }
  
  ~BlockStatement() {
  }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override {
    return mBlock.generateIR(context);
  }
};

} /* namespace yazyk */

#endif /* BlockStatement_h */
