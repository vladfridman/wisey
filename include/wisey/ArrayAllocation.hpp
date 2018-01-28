//
//  ArrayAllocation.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayAllocation_h
#define ArrayAllocation_h

#include "wisey/ArraySpecificTypeSpecifier.hpp"
#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents array allocation expression of the form: new int[20]
   */
  class ArrayAllocation : public IExpression {
    
    const ArraySpecificTypeSpecifier* mArraySpecificTypeSpecifier;
    
  public:
    
    ArrayAllocation(const ArraySpecificTypeSpecifier* arraySpecificTypeSpecifier);
    
    ~ArrayAllocation();
    
    /**
     * Allocates memory for the given type array
     */
    static llvm::Value* allocateArray(IRGenerationContext &context,
                                      const ArraySpecificType* arrayType);
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    bool isConstant() const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };

} /* namespace wisey */

#endif /* ArrayAllocation_h */
