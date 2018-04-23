//
//  ImmutableArrayOwnerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ImmutableArrayOwnerType_h
#define ImmutableArrayOwnerType_h

#include "wisey/IOwnerType.hpp"

namespace wisey {
  
  class ImmutableArrayType;
  
  /**
   * Represents owner type reference to the immutable array type
   */
  class ImmutableArrayOwnerType : public IOwnerType {
    
    const ImmutableArrayType* mImmutableArrayType;
    
  public:
    
    ImmutableArrayOwnerType(const ImmutableArrayType* immutableArrayType);
    
    ~ImmutableArrayOwnerType();
    
    void free(IRGenerationContext& context, llvm::Value* arrayPointer, int line) const override;
    
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
    
    bool isThread() const override;
    
    bool isNative() const override;
    
    bool isPointer() const override;

    bool isImmutable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;
    
    const IReferenceType* getReference() const override;
    
    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArguments,
                              int line) const override;
    
  };
  
} /* namespace wisey */

#endif /* ImmutableArrayOwnerType_h */