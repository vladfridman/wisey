//
//  CharConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef CharConstant_h
#define CharConstant_h

#include <string>

#include "IConstantExpression.hpp"
#include "IHasType.hpp"

namespace wisey {
  
  /**
   * Represents a constant character
   */
  class CharConstant : public IConstantExpression {
    std::string mValue;
    int mLine;
    
  public:
    
    CharConstant(std::string value, int line);
    
    ~CharConstant();
    
    int getLine() const override;

    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* CharConstant_h */

