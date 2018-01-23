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

#include "wisey/IReferenceType.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
class ArrayOwnerType;

/**
 * Represents the array type
 *
 * Arrays are wrapped in a Struct in order to keep track of reference count and array size
 * The Struct looks as follows:
 * {
 *   int_64 ref_count
 *   int_64 number_of_dimensions
 *   int_64 linear_size // equals to the product of all dimension sizes
 *   { int 64 dimention_1_size, int 64 dimention_2_size }
 *   [ dimention_1_size x [dimention_2_size x <element_type> ]]
 * }
 */
class ArrayType : public IReferenceType {
  
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
  
  /**
   * Returns the number of dimensions in this array
   */
  unsigned long getDimentionsSize() const;
  
  /**
   * Returns linear size of the array that equals to product of all dimention sizes
   */
  unsigned long getLinearSize() const;
  
  /**
   * Return array base type
   */
  const IType* getBaseType() const;
  
  /**
   * Return array size
   */
  unsigned long getSize() const;
  
  /**
   * Returns single array element type
   */
  const IType* getElementType() const;
  
  void incrementReferenceCount(IRGenerationContext& context,
                               llvm::Value* arrayPointer) const override;

  void decrementReferenceCount(IRGenerationContext& context,
                               llvm::Value* arrayPointer) const override;

  std::string getTypeName() const override;
  
  llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;

  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isOwner() const override;

  /**
   * Returns generic type for arrays
   */
  static llvm::PointerType* getGenericArrayType(IRGenerationContext& context);

private:
  
  llvm::Value* bitcastToGenericPointer(IRGenerationContext& context,
                                       llvm::Value* arrayPointer) const;
  
};
  
}

#endif /* ArrayType_h */
