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

#include "Catch.hpp"
#include "CompoundStatement.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Represents a try/catch statement
   */
  class TryCatchStatement : public IStatement {
    CompoundStatement* mTryCompoundStatement;
    std::vector<Catch*> mCatchList;
    int mLine;
    
  public:
    
    TryCatchStatement(CompoundStatement* tryCompoundStatement,
                      std::vector<Catch*> catchList,
                      int line);
    
    ~TryCatchStatement();
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* TryCatchStatement_h */

