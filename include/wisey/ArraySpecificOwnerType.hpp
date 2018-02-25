//
//  ArraySpecificOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArraySpecificOwnerType_h
#define ArraySpecificOwnerType_h

#include "wisey/ArraySpecificType.hpp"
#include "wisey/IOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents owner type reference to the ArraySpecificType
   */
  class ArraySpecificOwnerType : public IOwnerType {
    
    const ArraySpecificType* mArraySpecificType;
    
  public:
    
    ArraySpecificOwnerType(const ArraySpecificType* arraySpecificType);
    
    ~ArraySpecificOwnerType();
    
    /**
     * Return specific array type that this type is owner of
     */
    const ArraySpecificType* getArraySpecificType() const;
    
    void free(IRGenerationContext& context, llvm::Value* arrayPointer) const override;
    
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

    const IObjectType* getObjectType() const override;

  };
  
} /* namespace wisey */

#endif /* ArraySpecificOwnerType_h */
