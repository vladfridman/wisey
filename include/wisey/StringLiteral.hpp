//
//  StringLiteral.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef StringLiteral_h
#define StringLiteral_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
/**
 * Represents a string literal
 */
class StringLiteral : public IExpression {
  std::string mValue;
  
public:
  StringLiteral(std::string input) : mValue(unescape(input.substr(1, input.length() - 2))) { }
  
  ~StringLiteral() { }
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;

  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  std::string unescape(const std::string& input);
};
  
} /* namespace wisey */

#endif /* StringLiteral_h */
