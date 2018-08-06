//
//  HexConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef HexConstant_h
#define HexConstant_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a hexadecimal constant
   */
  class HexConstant : public IExpression {
    
    unsigned long mValue;
    int mLine;
    
  public:
    
    HexConstant(unsigned long value, int line);
    
    ~HexConstant();
    
    int getLine() const override;
    
    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* HexConstant_h */
