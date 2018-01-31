//
//  ArraySpecificType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArraySpecificType_h
#define ArraySpecificType_h

#include <list>

#include <llvm/IR/DerivedTypes.h>

#include "wisey/ArrayType.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
  class ArraySpecificOwnerType;
  
  /**
   * Represents the array type where each dimension is specified.
   *
   * This class is used for array allocation command e.g. new int[5]; 
   */
  class ArraySpecificType : public IType {
    
    const IType* mElementType;
    std::list<unsigned long> mDimensions;
    const ArraySpecificOwnerType* mArraySpecificOwnerType;
    
  public:
    
    ArraySpecificType(const IType* elementType, std::list<unsigned long> dimensions);
    
    ~ArraySpecificType();
    
    /**
     * Returns the owner type for this specific array type
     */
    const ArraySpecificOwnerType* getOwner() const;

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
    
  };
  
} /* namespace wisey */

#endif /* ArraySpecificType_h */
