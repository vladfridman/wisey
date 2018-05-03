//
//  LLVMPointerType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMPointerType_h
#define LLVMPointerType_h

#include <llvm/IR/Instructions.h>

#include "wisey/ILLVMType.hpp"

namespace wisey {
  
  class LLVMPointerOwnerType;
  
  /**
   * Represents an llvm pointer type
   */
  class LLVMPointerType : public ILLVMType {
    
    const ILLVMType* mBaseType;
    const LLVMPointerType* mPointerType;
    const LLVMPointerOwnerType* mPointerOwnerType;
    
    LLVMPointerType(const ILLVMType* baseType, unsigned long degree);

  public:
    
    static unsigned long LLVM_POINTER_MAX_DEGREE;

    static LLVMPointerType* create(const ILLVMType* baseType);
    
    ~LLVMPointerType();
    
    /**
     * Returns corresponding pointer owner type
     */
    const LLVMPointerOwnerType* getOwner() const;
    
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
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context, int line) const override;
    
    const LLVMPointerType* getPointerType(IRGenerationContext& context, int line) const override;

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;

  };
  
} /* namespace wisey */

#endif /* LLVMPointerType_h */
