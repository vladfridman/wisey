//
//  ByteConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ByteConstant_h
#define ByteConstant_h

#include <string>

#include "wisey/IConstantExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a byte constant
   */
  class ByteConstant : public IConstantExpression {
    int mValue;
    int mLine;
    
  public:
    
    ByteConstant(int value, int line);
    
    ~ByteConstant();
    
    int getLine() const override;
    
    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ByteConstant_h */
