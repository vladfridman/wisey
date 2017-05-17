//
//  EmptyStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef EmptyStatement_h
#define EmptyStatement_h

#include "wisey/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents an empty statement that does not do anything
 */
class EmptyStatement : public IStatement {

public:

  const static EmptyStatement EMPTY_STATEMENT;
  
  EmptyStatement() { }
  
  ~EmptyStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace yazyk */

#endif /* EmptyStatement_h */
