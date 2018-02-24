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
    
    bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    llvm::Value* castTo(IRGenerationContext& context,
                        llvm::Value* fromValue,
                        const IType* toType,
                        int line) const override;
    
    bool isOwner() const override;
    
    bool isReference() const override;
    
    bool isArray() const override;
    
    bool isController() const override;
    
    bool isInterface() const override;
    
    bool isModel() const override;
    
    bool isNode() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;

    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;

  };
  
} /* namespace wisey */

#endif /* ArrayExactType_h */
