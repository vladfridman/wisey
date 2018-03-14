//
//  DereferenceExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef DereferenceExpression_h
#define DereferenceExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents a dereferencing operation, it is only used for native code integration
   */
  class DereferenceExpression : public IExpression {
    IExpression* mExpression;
    
  public:
    
    DereferenceExpression(IExpression* expression);
    
    ~DereferenceExpression();
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* DereferenceExpression_h */

