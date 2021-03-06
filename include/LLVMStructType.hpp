//
//  LLVMStructType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMStructType_h
#define LLVMStructType_h

#include <llvm/IR/Constants.h>

#include "IType.hpp"
#include "LLVMPointerType.hpp"

namespace wisey {
  
  /**
   * Represents an llvm struct type
   */
  class LLVMStructType : public ILLVMType {
    
    llvm::StructType* mStructType;
    bool mIsExternal;
    std::vector<const IType*> mBodyTypes;
    const LLVMPointerType* mPointerType;
    
    LLVMStructType(llvm::StructType* structType, bool isExternal);

  public:
    
    ~LLVMStructType();
    
    /**
     * ::llvm::struct:: prefix used in referencing struct types
     */
    static std::string LLVM_STRUCT_PREFIX;
    
    /**
     * static method for llvm struct instantiation
     */
    static LLVMStructType* newLLVMStructType(llvm::StructType* structType);
    
    /**
     * static method for external llvm struct instantiation
     */
    static LLVMStructType* newExternalLLVMStructType(llvm::StructType* structType);

    /**
     * Sets body types of this struct type
     */
    void setBodyTypes(IRGenerationContext& context, std::vector<const IType*> bodyTypes);
    
    /**
     * Tells whether this is an externally defined struct
     */
    bool isExternal() const;
    
    std::string getTypeName() const override;
    
    llvm::StructType* getLLVMType(IRGenerationContext& context) const override;
    
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

    const LLVMPointerType* getPointerType(IRGenerationContext& context, int line) const override;
    
    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;

  };
  
} /* namespace wisey */

#endif /* LLVMStructType_h */
