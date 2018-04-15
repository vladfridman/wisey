//
//  ThreadExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadExpression_h
#define ThreadExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
  /**
   * Represents the current thread in a wisey program.
   */
  class ThreadExpression : public IExpression {
    int mLine;

  public:
    
    ThreadExpression(int line);
    
    ~ThreadExpression();
    
    /**
     * Variable name referencing the current thread
     */
    static std::string THREAD;
    
    /**
     * Variable name referencing current thread's call stack
     */
    static std::string CALL_STACK;
    
    int getLine() const override;

    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ThreadExpression_h */

