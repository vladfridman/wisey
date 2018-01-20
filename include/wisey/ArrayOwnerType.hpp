//
//  ArrayOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayOwnerType_h
#define ArrayOwnerType_h

#include "wisey/ArrayType.hpp"
#include "wisey/IArrayType.hpp"

namespace wisey {
  
  /**
   * Represents owner type reference to the array type
   */
  class ArrayOwnerType : public IArrayType {
    
    const ArrayType* mArrayType;
    
  public:
    
    ArrayOwnerType(const ArrayType* arrayType);
    
    ~ArrayOwnerType();
    
    /**
     * Return array type that this type is owner of
     */
    const ArrayType* getArrayType() const;
    
    /**
     * Deallocates memory occupied by elements of this array when every element is an owner
     */
    void free(IRGenerationContext& context, llvm::Value* arrayPointer) const;

    const IType* getBaseType() const override;
    
    unsigned long getSize() const override;
    
    const IType* getScalarType() const override;

    std::string getTypeName() const override;
    
    llvm::StructType* getLLVMType(IRGenerationContext& context) const override;
    
    TypeKind getTypeKind() const override;
    
    bool canCastTo(const IType* toType) const override;
    
    bool canAutoCastTo(const IType* toType) const override;
    
    llvm::Value* castTo(IRGenerationContext& context,
                        llvm::Value* fromValue,
                        const IType* toType,
                        int line) const override;
        
  };

} /* namespace wisey */

#endif /* ArrayOwnerType_h */
