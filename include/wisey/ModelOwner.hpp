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
  const PointerType* mPointerType;

public:
  
  ModelOwner(Model* model);
  
  ~ModelOwner();
  
  Model* getObjectType() const override;
  
  std::string getTypeName() const override;
  
  llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
  
  void free(IRGenerationContext& context, llvm::Value* value) const override;

  bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isPrimitive() const override;

  bool isOwner() const override;
  
  bool isReference() const override;
  
  bool isArray() const override;
  
  bool isFunction() const override;
  
  bool isPackage() const override;

  bool isController() const override;
  
  bool isInterface() const override;
  
  bool isModel() const override;
  
  bool isNode() const override;
  
  bool isThread() const override;
  
  bool isNative() const override;

  llvm::Function* getDestructorFunction(IRGenerationContext& context) const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void createLocalVariable(IRGenerationContext& context, std::string name) const override;
  
  void createFieldVariable(IRGenerationContext& context,
                           std::string name,
                           const IConcreteObjectType* object) const override;
  
  void createParameterVariable(IRGenerationContext& context,
                               std::string name,
                               llvm::Value* value) const override;

  const ArrayType* getArrayType(IRGenerationContext& context) const override;
  
  const PointerType* getPointerType() const override;

};
  
} /* namespace wisey */

#endif /* ModelOwner_h */
