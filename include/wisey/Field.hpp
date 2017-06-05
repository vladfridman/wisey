//
//  Field.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Field_h
#define Field_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IType.hpp"

namespace wisey {

/**
 * Represents one field in a model
 */
class Field {
  IType* mType;
  std::string mName;
  unsigned long mIndex;
  ExpressionList mArguments;

public:
  
  Field(IType* type, std::string name, unsigned long index, ExpressionList arguments) :
  mType(type), mName(name), mIndex(index), mArguments(arguments) { }
  
  ~Field();
  
  IType* getType() const;

  std::string getName() const;
  
  unsigned long getIndex() const;
  
  ExpressionList getArguments() const;
};
  
} /* namespace wisey */

#endif /* Field_h */
