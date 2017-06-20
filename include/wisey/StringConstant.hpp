//
//  StringConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef StringConstant_h
#define StringConstant_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
/**
 * Represents a constant string
 */
class StringConstant : public IExpression {
  std::string mValue;
  
public:
  StringConstant(std::string input) : mValue(unescape(input.substr(1, input.length() - 2))) { }
  
  ~StringConstant() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;

  bool existsInOuterScope(IRGenerationContext& context) const override;

private:
  
  std::string unescape(const std::string& input);
};
  
} /* namespace wisey */

#endif /* StringConstant_h */
