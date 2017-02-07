//
//  StringConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef StringConstant_h
#define StringConstant_h

#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents a constant string
 */
class StringConstant : public IExpression {
  std::string mValue;
  
public:
  StringConstant(std::string input) : mValue(unescape(input.substr(1, input.length() - 2))) { }
  
  ~StringConstant() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  std::string unescape(const std::string& input);
};
  
} /* namespace yazyk */

#endif /* StringConstant_h */
