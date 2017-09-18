//
//  PrintOutStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PrintOutStatement_h
#define PrintOutStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {
  
class IRGenerationContext;
  
/**
 * Represents a printout statement for printing things to stdout stream
 */
class PrintOutStatement : public IStatement {
  std::vector<IExpression*> mExpressionList;
  
public:
  
  PrintOutStatement(std::vector<IExpression*> expressionList) : mExpressionList(expressionList) { }
  
  ~PrintOutStatement();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* PrintOutStatement_h */
