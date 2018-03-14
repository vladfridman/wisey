//
//  FloatConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef FloatConstant_h
#define FloatConstant_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a float constant
   */
  class FloatConstant: public IExpression {
    double mValue;
    
  public:
    
    FloatConstant(double value) : mValue(value) { }
    
    ~FloatConstant() {}
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* FloatConstant_h */

