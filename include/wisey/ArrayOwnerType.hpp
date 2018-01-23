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
#include "wisey/IOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents owner type reference to the array type
   */
  class ArrayOwnerType : public IArrayType, public IOwnerType {
    
    const ArrayType* mArrayType;
    
  public:
    
    ArrayOwnerType(const ArrayType* arrayType);
    
    ~ArrayOwnerType();
    
    /**
     * Return array type that this type is owner of
     */
    const ArrayType* getArrayType() const;
    
    void free(IRGenerationContext& context, llvm::Value* arrayPointer) const override;

    const IType* getBaseType() const override;
    
    unsigned long getSize() const override;
    
    const IType* getScalarType() const override;

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
        
  };

} /* namespace wisey */

#endif /* ArrayOwnerType_h */
