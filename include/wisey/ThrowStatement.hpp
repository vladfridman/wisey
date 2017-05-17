//
//  ThrowStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ThrowStatement_h
#define ThrowStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents a throw statement that throws result of the given expression
 */
class ThrowStatement : public IStatement {
  IExpression& mExpression;
  
public:
  
  ThrowStatement(IExpression& conditionExpression) : mExpression(conditionExpression) { }
  
  ~ThrowStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace yazyk */


#endif /* ThrowStatement_h */
