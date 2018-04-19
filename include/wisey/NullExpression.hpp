//
//  NullExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NullExpression_h
#define NullExpression_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents the null
   */
  class NullExpression : public IExpression {
    int mLine;
    
  public:
    
    NullExpression(int line);
    
    ~NullExpression();
    
    int getLine() const override;

    llvm::Constant* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
}

#endif /* NullExpression_h */

