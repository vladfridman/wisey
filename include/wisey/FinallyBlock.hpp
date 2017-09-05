//
//  FinallyBlock.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FinallyBlock_h
#define FinallyBlock_h

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {

/**
 * Represents list of statements executed as part of the finally clause in try/catch construct
 */
class FinallyBlock : public IStatement {
  StatementList mStatements;
  
public:
  FinallyBlock() { }
  
  ~FinallyBlock() { }
  
  StatementList& getStatements();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* FinallyBlock_h */
