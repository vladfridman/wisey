//
//  ArrayType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayType_h
#define ArrayType_h

#include <llvm/IR/Instructions.h>

#include "wisey/IArrayType.hpp"

namespace wisey {
  
class ArrayOwnerType;

/**
 * Represents the array type
 */
class ArrayType : public IArrayType {
  
  const IType* mBaseType;
  unsigned long mSize;
  const ArrayOwnerType* mArrayOwnerType;
  
public:
  
  ArrayType(const IType* baseType, unsigned long size);
  
  ~ArrayType();

  /**
   * Returns the owner type for this array type
   */
  const ArrayOwnerType* getOwner() const;
  
  const IType* getBaseType() const override;
  
  unsigned long getSize() const override;
  
  const IType* getScalarType() const override;
  
  void free(IRGenerationContext& context, llvm::Value* arrayPointer) const override;
  
  void decrementReferenceCount(IRGenerationContext& context, 
                               llvm::Value* arrayPointer) const override;

  std::string getTypeName() const override;
  
  llvm::ArrayType* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;

  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
private:
  
  unsigned long getLinearSize() const;
  
  llvm::Value* bitcastToGenericArray(IRGenerationContext& context, llvm::Value* arrayPointer) const;
  
};
  
}

#endif /* ArrayType_h */
