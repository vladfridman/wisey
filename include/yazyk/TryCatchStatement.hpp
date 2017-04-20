//
//  TryCatchStatement.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TryCatchStatement_h
#define TryCatchStatement_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents a try/catch statement
 */
class TryCatchStatement : public IStatement {
  IStatement& mTryBlock;
  std::vector<Catch*> mCatchList;
  
public:
  
  TryCatchStatement(IStatement& tryBlock, std::vector<Catch*> catchList)
  : mTryBlock(tryBlock), mCatchList(catchList) { }
  
  ~TryCatchStatement() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override { return NULL; }
};
  
} /* namespace yazyk */

#endif /* TryCatchStatement_h */
