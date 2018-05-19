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

    void free(IRGenerationContext& context,
              llvm::Value* arrayPointer,
              llvm::Value* exception,
              int line) const override;
    
    std::string getTypeName() const override;
    
    llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
    
    bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    llvm::Value* castTo(IRGenerationContext& context,
                        llvm::Value* fromValue,
                        const IType* toType,
                        int line) const override;
    
    bool isPrimitive() const override;

    bool isOwner() const override;
    
    bool isReference() const override;
    
    bool isArray() const override;
    
    bool isFunction() const override;
    
    bool isPackage() const override;

    bool isController() const override;
    
    bool isInterface() const override;
    
    bool isModel() const override;
    
    bool isNode() const override;
    
    bool isNative() const override;
    
    bool isPointer() const override;

    bool isImmutable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context,
                             std::string name,
                             int line) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object,
                             int line) const override;

    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value,
                                 int line) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context, int line) const override;

    const IReferenceType* getReference() const override;
    
    llvm::Instruction* inject(IRGenerationContext& context,
                              const std::vector<InjectionArgument*> injectionArgumentList,
                              int line) const override;

  };
  
} /* namespace wisey */

#endif /* ArrayExactOwnerType_h */
