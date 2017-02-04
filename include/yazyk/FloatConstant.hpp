//
//  FloatConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef FloatConstant_h
#define FloatConstant_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents a float constant
 */
class FloatConstant: public IExpression, IHasType {
  double mValue;
  
public:
  
  FloatConstant(double value) : mValue(value) { }

  ~FloatConstant() {}
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType() const override;
};

} /* namespace yazyk */

#endif /* FloatConstant_h */
