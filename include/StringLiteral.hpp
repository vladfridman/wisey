//
//  StringLiteral.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef StringLiteral_h
#define StringLiteral_h

#include "IConstantExpression.hpp"

namespace wisey {
  
  /**
   * Represents a string literal
   */
  class StringLiteral : public IConstantExpression {
    std::string mValue;
    int mLine;
    
  public:
    
    StringLiteral(std::string input, int line);
    
    ~StringLiteral();
    
    int getLine() const override;

    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    std::string unescape(const std::string& input);
  };
  
} /* namespace wisey */

#endif /* StringLiteral_h */

