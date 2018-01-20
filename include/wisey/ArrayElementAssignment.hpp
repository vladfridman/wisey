//
//  ArrayElementAssignment.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayElementAssignment_h
#define ArrayElementAssignment_h

#include <llvm/IR/Instruction.h>

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Class containing method for assigning array elements of owner, reference and primitive types
   */
  class ArrayElementAssignment {

  public:
    
    /**
     * Generate array element assignment IR code
     */
    static llvm::Value* generateElementAssignment(IRGenerationContext& context,
                                                  const IType* elementType,
                                                  IExpression* assignToExpression,
                                                  llvm::Value* elementStore,
                                                  int line);

  private:
    
    static llvm::Value* generateOwnerElementAssignment(IRGenerationContext& context,
                                                       const IType* elementType,
                                                       IExpression* assignToExpression,
                                                       llvm::Value* elementStore,
                                                       int line);

    static llvm::Value* generateReferenceElementAssignment(IRGenerationContext& context,
                                                           const IType* elementType,
                                                           IExpression* assignToExpression,
                                                           llvm::Value* elementStore,
                                                           int line);

    static llvm::Value* generatePrimitiveElementAssignment(IRGenerationContext& context,
                                                           const IType* elementType,
                                                           IExpression* assignToExpression,
                                                           llvm::Value* elementStore,
                                                           int line);

  };
  
}
#endif /* ArrayElementAssignment_h */
