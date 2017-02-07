//
//  IntConstant.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IntConstant_h
#define IntConstant_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents an integer constant
 */
class IntConstant : public IExpression, IHasType {

  long mValue;
  
public:
  IntConstant(long value) : mValue(value) { }

  ~IntConstant() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* IntConstant_h */
