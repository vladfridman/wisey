//
//  SwitchStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef SwitchStatement_h
#define SwitchStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/CaseStatement.hpp"
#include "wisey/DefaultCaseStatement.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Data structure to hold information about cases inside a SWITCH statement
   */
  struct SwitchCases {
    
    std::vector<CaseStatement*> caseStatements;
    
    DefaultCaseStatement* defaultStatement;
  };
  
  /**
   * Represents a switch statement
   * switch (expression) statement;
   */
  class SwitchStatement : public IStatement {
    IExpression* mCondition;
    SwitchCases* mSwitchCases;
    
  public:
    
    SwitchStatement(IExpression* condition, SwitchCases* switchCases);
    
    ~SwitchStatement();
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  private:
    
    /**
     * Generate IR for all the cases excluding the default one
     */
    void generateCasesIR(IRGenerationContext& context,
                         llvm::BasicBlock* switchDefault,
                         llvm::BasicBlock* switchEpilog,
                         llvm::SwitchInst* switchInst) const;
    
    /**
     * Generate IR for all the default case
     */
    void generateDefaultCaseIR(IRGenerationContext& context,
                               llvm::BasicBlock* switchDefault,
                               llvm::BasicBlock* switchEpilog) const;
  };
  
} /* namespace wisey */

#endif /* SwitchStatement_h */

