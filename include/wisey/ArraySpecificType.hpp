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
#include "wisey/IExpression.hpp"
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
    std::list<const IExpression*> mDimensions;
    const ArraySpecificOwnerType* mArraySpecificOwnerType;
    
  public:
    
    ArraySpecificType(const IType* elementType, std::list<const IExpression*> dimensions);
    
    ~ArraySpecificType();
    
    /**
     * Returns the owner type for this specific array type
     */
    const ArraySpecificOwnerType* getOwner() const;
    
    /**
     * Returns the number of dimensions in this array
     */
    unsigned long getNumberOfDimensions() const;
    
    /**
     * Returns single array element type
     */
    const IType* getElementType() const;
    
    /**
     * Computes sizes of all sub arrays and array element sizes
     */
    std::list<std::tuple<llvm::Value*, llvm::Value*>> computeArrayAllocData(IRGenerationContext&
                                                                            context) const;
    
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

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;

    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;

  private:
    
    void checkDimensionType(IRGenerationContext& context, const IType* type) const;
    
  };
  
} /* namespace wisey */

#endif /* ArraySpecificType_h */
