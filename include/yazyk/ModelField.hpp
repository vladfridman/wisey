//
//  ModelField.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelField_h
#define ModelField_h

#include <llvm/IR/Instructions.h>

namespace yazyk {

/**
 * Represents one field in a model
 */
class ModelField {
  IType* mType;
  int mIndex;

public:
  
  ModelField(IType* type, int index) : mType(type), mIndex(index) { }
  
  ~ModelField() { }
  
  int getIndex() { return mIndex; }
  
  IType* getType() const { return mType; }
};
  
} /* namespace yazyk */

#endif /* ModelField_h */
