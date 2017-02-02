//
//  Model.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <llvm/IR/Instructions.h>

#include "yazyk/ModelField.hpp"

namespace yazyk {

/**
 * Contains information about a MODEL including the llvm::StructType and field information
 */
class Model {
  llvm::StructType* mStructType;
  std::map<std::string, ModelField*> *mFields;
  
public:
  
  Model(llvm::StructType* structType, std::map<std::string, ModelField*>* fields) :
    mStructType(structType), mFields(fields) {}
  
  ~Model();
  
  llvm::StructType* getStructType();
  
  ModelField* findField(std::string fieldName);
};

} /* namespace yazyk */

#endif /* Model_h */
