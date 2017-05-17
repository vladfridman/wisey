//
//  ReturnVoidStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef ReturnVoidStatement_h
#define ReturnVoidStatement_h

#include "wisey/IStatement.hpp"

namespace wisey {

/**
 * Represents a return that does not return any expression
 */
class ReturnVoidStatement : public IStatement {

public:

  ReturnVoidStatement() { }
  
  ~ReturnVoidStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* ReturnVoidStatement_h */
