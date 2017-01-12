//
//  SwitchStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef SwitchStatement_h
#define SwitchStatement_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {
  
/**
 * Represents a switch statement
 * switch (expression) statement;
 */
class SwitchStatement : public IStatement {
  IExpression& mCondition;
  std::vector<CaseStatement*> mCaseStatementList;
  
public:
  
  SwitchStatement(IExpression& condition, std::vector<CaseStatement*>& caseStatementList)
  : mCondition(condition), mCaseStatementList(caseStatementList) { }
  
  ~SwitchStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* SwitchStatement_h */
