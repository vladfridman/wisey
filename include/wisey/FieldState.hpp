//
//  FieldState.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldState_h
#define FieldState_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IField.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents a state field in a controller or a node
 */
class FieldState : public IField {
  const IType* mType;
  std::string mName;
  unsigned long mIndex;
  ExpressionList mArguments;
  
public:
  
  FieldState(const IType* type, std::string name, unsigned long index, ExpressionList arguments) :
  mType(type), mName(name), mIndex(index), mArguments(arguments) { }
  
  ~FieldState();
  
  const IType* getType() const override;
  
  std::string getName() const override;
  
  unsigned long getIndex() const override;
  
  ExpressionList getArguments() const override;
  
};
  
} /* namespace wisey */

#endif /* FieldState_h */
