//
//  Variable.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Variable_h
#define Variable_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IType.hpp"

namespace yazyk {

/**
 * Memory storage types for storing variables
 */
typedef enum VariableStorageTypeEnum {
  STACK_VARIABLE,
  HEAP_VARIABLE,
  HEAP_VARIABLE_UNINITIALIZED,
} VariableStorageType;

/**
 * Contains information about a variable
 */
class Variable {
  llvm::Value* mValue;
  IType* mType;
  VariableStorageType mStorageType;
  
public:
  
  Variable(llvm::Value* value, IType* type, VariableStorageType storageType)
    : mValue(value), mType(type), mStorageType(storageType) { }
  
  ~Variable() { }
  
  llvm::Value* getValue() { return mValue; }
  
  VariableStorageType getStorageType() { return mStorageType; }
  
  IType* getType() { return mType; }
};
  
} /* namespace yazyk */

#endif /* Variable_h */
