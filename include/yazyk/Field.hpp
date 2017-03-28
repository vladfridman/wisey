//
//  Field.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Field_h
#define Field_h

#include <llvm/IR/Instructions.h>

namespace yazyk {

/**
 * Represents one field in a model
 */
class Field {
  IType* mType;
  unsigned long mIndex;

public:
  
  Field(IType* type, unsigned long index) : mType(type), mIndex(index) { }
  
  ~Field() { }
  
  unsigned long getIndex() { return mIndex; }
  
  IType* getType() const { return mType; }
};
  
} /* namespace yazyk */

#endif /* Field_h */
