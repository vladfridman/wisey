//
//  BooleanConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanConstant_h
#define BooleanConstant_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {
  
/**
 * Represents a constant boolean
 */
class BooleanConstant : public IExpression, IHasType {
  bool mValue;
  
public:
  
  BooleanConstant(bool value) : mValue(value) { }
  
  ~BooleanConstant() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
};
  
} /* namespace yazyk */

#endif /* BooleanConstant_h */