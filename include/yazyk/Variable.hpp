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
  VariableStorageType mStorageType;
  
public:
  
  Variable(llvm::Value* value, VariableStorageType storageType)
    : mValue(value), mStorageType(storageType) { }
  
  Variable() : mValue(NULL), mStorageType(STACK_VARIABLE) {}
  
  ~Variable() { }
  
  llvm::Value* getValue() { return mValue; }
  
  VariableStorageType getStorageType() { return mStorageType; }
};
  
} /* namespace yazyk */

#endif /* Variable_h */
