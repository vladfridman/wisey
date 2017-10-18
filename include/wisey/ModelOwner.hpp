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
  
  std::string getName() const override;
  
  llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmContext) const override;
  
  void free(IRGenerationContext& context, llvm::Value* value) const override;

  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;

};
  
} /* namespace wisey */

#endif /* ModelOwner_h */
