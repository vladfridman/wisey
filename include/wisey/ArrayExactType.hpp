//
//  ArrayExactType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayExactType_h
#define ArrayExactType_h

#include <list>

#include <llvm/IR/DerivedTypes.h>

#include "wisey/ArrayType.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
  class ArrayExactOwnerType;
  
  /**
   * Represents the array type where each dimension is exactly defined and has a constant value.
   *
   * This class is used for static array allocation command e.g. { 1, 2, 3, 4 };
   */
  class ArrayExactType : public IType {
    
    const IType* mElementType;
    std::list<unsigned long> mDimensions;
    const ArrayExactOwnerType* mArrayExactOwnerType;
    
  public:
    
    ArrayExactType(const IType* elementType, std::list<unsigned long> dimensions);
    
    ~ArrayExactType();
    
    /**
     * Returns the owner type for this exact array type
     */
    const ArrayExactOwnerType* getOwner() const;
    
    /**
     * Returns corresponding ArrayType
     */
    ArrayType* getArrayType(IRGenerationContext& context) const;
    
    /**
     * Returns the number of dimensions in this array
     */
    unsigned long getNumberOfDimensions() const;
    
    /**
     * Return array dimensions
     */
    std::list<unsigned long> getDimensions() const;
    
    /**
     * Returns single array element type
     */
    const IType* getElementType() const;
    
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

#endif /* ArrayExactType_h */
