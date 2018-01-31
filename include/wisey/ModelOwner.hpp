//
//  ModelOwner.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelOwner_h
#define ModelOwner_h

#include "wisey/Model.hpp"

namespace wisey {

/**
 * Represents an owner type for a model
 */
class ModelOwner : public IObjectOwnerType {

  Model* mModel;
  
public:
  
  ModelOwner(Model* model);
  
  ~ModelOwner();
  
  Model* getObject() const override;
  
  std::string getTypeName() const override;
  
  llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
  
  llvm::Value* computeSize(IRGenerationContext& context) const override;

  void free(IRGenerationContext& context, llvm::Value* value) const override;

  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isOwner() const override;

  llvm::Function* getDestructorFunction(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* ModelOwner_h */
