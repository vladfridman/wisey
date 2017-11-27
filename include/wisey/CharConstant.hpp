//
//  CharConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef CharConstant_h
#define CharConstant_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {

/**
 * Represents a constant character
 */
class CharConstant : public IExpression {
  char mValue;
  
public:

  CharConstant(char value) : mValue(value) { }
  
  ~CharConstant() {}
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Constant* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;

  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* CharConstant_h */
