//
//  DereferenceExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef DereferenceExpression_h
#define DereferenceExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/ILLVMType.hpp"

namespace wisey {
  
  /**
   * Represents ::llvm::dereference expression
   */
  class DereferenceExpression : public IExpression {
    IExpression* mExpression;
    int mLine;
    
  public:
    
    DereferenceExpression(IExpression* expression, int line);
    
    ~DereferenceExpression();
    
    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const ILLVMType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  private:
    
    void checkExpression(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* DereferenceExpression_h */
