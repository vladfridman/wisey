//
//  Model.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "yazyk/IType.hpp"
#include "yazyk/ModelField.hpp"

namespace yazyk {

/**
 * Contains information about a MODEL including the llvm::StructType and field information
 */
class Model : public IType {
  std::string mName;
  llvm::StructType* mStructType;
  std::map<std::string, ModelField*>* mFields;
  
public:
  
  Model(std::string name,
        llvm::StructType* structType,
        std::map<std::string, ModelField*>* fields) :
    mName(name), mStructType(structType), mFields(fields) {}
  
  ~Model();
  
  /**
   * Looks for a field with a given name in the model
   */
  ModelField* findField(std::string fieldName);
  
  /**
   * Gets a set of field names and returns the ones that are missing
   */
  std::vector<std::string> getMissingFields(std::set<std::string> givenFields);

  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
};

} /* namespace yazyk */

#endif /* Model_h */
