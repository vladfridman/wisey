//
//  ArrayExactOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayExactOwnerType_h
#define ArrayExactOwnerType_h

#include "wisey/ArrayExactType.hpp"
#include "wisey/IOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents owner type reference to the ArrayExactType
   */
  class ArrayExactOwnerType : public IOwnerType {
    
    const ArrayExactType* mArrayExactType;
    
  public:
    
    ArrayExactOwnerType(const ArrayExactType* arrayExactType);
    
    ~ArrayExactOwnerType();
    
    /**
     * Return exact array type that this type is owner of
     */
    const ArrayExactType* getArrayExactType() const;
    
    void free(IRGenerationContext& context, llvm::Value* arrayPointer) const override;
    
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
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  };
  
} /* namespace wisey */

#endif /* ArrayExactOwnerType_h */
