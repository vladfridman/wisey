//
//  ArrayAllocation.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayAllocation_h
#define ArrayAllocation_h

#include "ArraySpecificTypeSpecifier.hpp"
#include "ArraySpecificType.hpp"
#include "IExpression.hpp"

namespace wisey {
  
  /**
   * Represents array allocation expression of the form: new int[20]
   */
  class ArrayAllocation : public IExpression {
    
    const ArraySpecificTypeSpecifier* mArraySpecificTypeSpecifier;
    int mLine;
    
  public:
    
    ArrayAllocation(const ArraySpecificTypeSpecifier* arraySpecificTypeSpecifier, int line);
    
    ~ArrayAllocation();
    
    /**
     * Allocate array and set it to all zeros given the array type
     */
    static llvm::Value* allocateArray(IRGenerationContext &context,
                                      const ArraySpecificType* arrayType,
                                      int line);

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;
    
  private:
    
    static void initializeEmptyArray(IRGenerationContext& context,
                                     llvm::Value* arrayStructPointer,
                                     std::list<std::tuple<llvm::Value*, llvm::Value*>> arrayData);

  };

} /* namespace wisey */

#endif /* ArrayAllocation_h */
