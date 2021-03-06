//
//  IntConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IntConstant_h
#define IntConstant_h

#include "IConstantExpression.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents an integer constant
   */
  class IntConstant : public IConstantExpression {
    
    long mValue;
    int mLine;

  public:
    
    IntConstant(long value, int line);
    
    ~IntConstant();
    
    int getLine() const override;

    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* IntConstant_h */

