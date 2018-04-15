//
//  Assignment.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Assignment_h
#define Assignment_h

#include "wisey/Identifier.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents an assignment expression
   */
  class Assignment : public IExpression {
    IExpression* mIdentifier;
    IExpression* mExpression;
    int mLine;
    
  public:
    
    Assignment(IExpression* identifier, IExpression* expression, int line);
    
    ~Assignment();
    
    int getLine() const override;

    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* generateIRForHeapVariable(IRGenerationContext& context) const;
    
    llvm::Value* generateIRForLocalPrimitiveVariable(IRGenerationContext& context) const;
  };
  
}

#endif /* Assignment_h */

