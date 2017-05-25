//
//  EmptyStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef EmptyStatement_h
#define EmptyStatement_h

#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents an empty statement that does not do anything
 */
class EmptyStatement : public IStatement {

public:

  const static EmptyStatement EMPTY_STATEMENT;
  
  EmptyStatement() { }
  
  ~EmptyStatement() { }
  
  void prototypeObjects(IRGenerationContext& context) const override;
 
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* EmptyStatement_h */
