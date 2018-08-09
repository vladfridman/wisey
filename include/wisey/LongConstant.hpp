//
//  LongConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LongConstant_h
#define LongConstant_h

#include "wisey/IConstantExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a long constant
   */
  class LongConstant : public IConstantExpression {
    
    long long mValue;
    int mLine;
    
  public:
    
    LongConstant(long long value, int line);
    
    ~LongConstant();
    
    int getLine() const override;
    
    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* LongConstant_h */

