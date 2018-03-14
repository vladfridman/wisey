//
//  BooleanConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanConstant_h
#define BooleanConstant_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a constant boolean
   */
  class BooleanConstant : public IExpression {
    bool mValue;
    
  public:
    
    BooleanConstant(bool value) : mValue(value) { }
    
    ~BooleanConstant() {}
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* BooleanConstant_h */

