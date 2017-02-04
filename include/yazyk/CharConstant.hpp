//
//  CharConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef CharConstant_h
#define CharConstant_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents a constant character
 */
class CharConstant : public IExpression, IHasType {
  char mValue;
  
public:

  CharConstant(char value) : mValue(value) { }
  
  ~CharConstant() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType() const override;
};
  
} /* namespace yazyk */

#endif /* CharConstant_h */
