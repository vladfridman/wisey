//
//  PrintErrStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PrintErrStatement_h
#define PrintErrStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IPrintStatement.hpp"

namespace wisey {
  
class IRGenerationContext;
  
/**
 * Represents a printout statement for printing things to stdout stream
 */
class PrintErrStatement : public IPrintStatement {
  std::vector<IExpression*> mExpressionList;
  
public:
  
  PrintErrStatement(std::vector<IExpression*> expressionList);
  
  ~PrintErrStatement();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* PrintErrStatement_h */
