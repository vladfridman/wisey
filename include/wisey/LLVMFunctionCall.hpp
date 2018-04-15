//
//  LLVMFunctionCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionCall_h
#define LLVMFunctionCall_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents a static method call of the form MModel.someMethod() where MModel is object type
   */
  class LLVMFunctionCall : public IExpression {
    std::string mFunctionName;
    ExpressionList mArguments;
    int mLine;
    
  public:
    
    LLVMFunctionCall(std::string functionName, ExpressionList arguments, int line);
    
    ~LLVMFunctionCall();
    
    int getLine() const override;

    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionCall_h */
