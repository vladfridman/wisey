//
//  LLVMFunctionType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionType_h
#define LLVMFunctionType_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/ILLVMType.hpp"
#include "wisey/LLVMPointerType.hpp"

namespace wisey {
  
  /**
   * Represents an llvm native function type
   */
  class LLVMFunctionType : public ILLVMType {
    
    const ILLVMType* mReturnType;
    std::vector<const ILLVMType*> mArgumentTypes;
    const LLVMPointerType* mPointerType;
    
  public:
    
    LLVMFunctionType(const ILLVMType* returnType, std::vector<const ILLVMType*> argumentTypes);
    
    ~LLVMFunctionType();
    
    /**
     * Return function argument types
     */
    std::vector<const ILLVMType*> getArgumentTypes() const;
    
    /**
     * Returns function return type
     */
    const ILLVMType* getReturnType() const;
    
    std::string getTypeName() const override;
    
    llvm::FunctionType* getLLVMType(IRGenerationContext& context) const override;
    
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
    
    const LLVMPointerType* getPointerType() const override;
    
    const ILLVMType* getDereferenceType() const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionType_h */
