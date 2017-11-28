//
//  TryCatchStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TryCatchStatement_h
#define TryCatchStatement_h

#include <llvm/IR/Instructions.h>

#include "wisey/Catch.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents a try/catch statement
 */
class TryCatchStatement : public IStatement {
  IStatement* mTryBlock;
  std::vector<Catch*> mCatchList;
  
public:
  
  TryCatchStatement(IStatement* tryBlock, std::vector<Catch*> catchList);
  
  ~TryCatchStatement();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* TryCatchStatement_h */
