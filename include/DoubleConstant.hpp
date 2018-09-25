//
//  DoubleConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef DoubleConstant_h
#define DoubleConstant_h

#include "IConstantExpression.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a double constant
   */
  class DoubleConstant : public IConstantExpression {
    long double mValue;
    int mLine;
    
  public:
    
    DoubleConstant(long double value, int line);
    
    ~DoubleConstant();
    
    int getLine() const override;

    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* DoubleConstant_h */

