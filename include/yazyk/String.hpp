//
//  String.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef String_h
#define String_h

#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents a constant string
 */
class String : public IExpression {
  std::string mValue;
  
public:
  String(std::string input) : mValue(unescape(input.substr(1, input.length() - 2))) { }
  
  ~String() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
  
private:
  std::string unescape(const std::string& input);
};
  
} /* namespace yazyk */

#endif /* String_h */
