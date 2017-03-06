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
#include "yazyk/Method.hpp"
#include "yazyk/ModelField.hpp"

namespace yazyk {

/**
 * Contains information about a MODEL including the llvm::StructType and field information
 */
class Model : public IType {
  std::string mName;
  llvm::StructType* mStructType;
  std::map<std::string, ModelField*>* mFields;
  std::map<std::string, Method*>* mMethods;
  
public:
  
  Model(std::string name,
        llvm::StructType* structType,
        std::map<std::string, ModelField*>* fields,
        std::map<std::string, Method*>* methods) :
    mName(name), mStructType(structType), mFields(fields), mMethods(methods) {}
  
  ~Model();
  
  /**
   * Looks for a field with a given name in the model
   */
  ModelField* findField(std::string fieldName) const;
  
  /**
   * Gets a set of field names and returns the ones that are missing
   */
  std::vector<std::string> getMissingFields(std::set<std::string> givenFields) const;
  
  /**
   * Findsa method with a given name
   */
  Method* findMethod(std::string methodName) const;

  /**
   * Returns the name of the vTable global varaible
   */
  std::string getVTableName() const;
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canCastLosslessTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
};

} /* namespace yazyk */

#endif /* Model_h */
