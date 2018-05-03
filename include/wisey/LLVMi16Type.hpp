//
//  LLVMi16Type.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMi16Type_h
#define LLVMi16Type_h

#include "wisey/ILLVMPrimitiveType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  class PointerType;
  
  /**
   * Represents a llvm primitive iteger type i16
   */
  class LLVMi16Type : public ILLVMPrimitiveType {
    
    const LLVMPointerType* mPointerType;
    
  public:
    
    LLVMi16Type();
    
    ~LLVMi16Type();
    
    std::string getTypeName() const override;
    
    llvm::Type* getLLVMType(IRGenerationContext& context) const override;
    
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
    
    const ILLVMTypeSpecifier* newTypeSpecifier(int line) const override;

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;
  };
  
} /* namespace wisey */

#endif /* LLVMi16Type_h */
