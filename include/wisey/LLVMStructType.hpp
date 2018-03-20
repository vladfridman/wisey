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

#include "wisey/ILLVMType.hpp"
#include "wisey/LLVMPointerType.hpp"

namespace wisey {
  
  /**
   * Represents an llvm struct type
   */
  class LLVMStructType : public ILLVMType {
    
    llvm::StructType* mStructType;
    std::vector<const ILLVMType*> mBodyTypes;
    const LLVMPointerType* mPointerType;
    
  public:
    
    LLVMStructType(llvm::StructType* structType);
    
    ~LLVMStructType();
    
    /**
     * Sets body types of this struct type
     */
    void setBodyTypes(IRGenerationContext& context, std::vector<const ILLVMType*> bodyTypes);
    
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

#endif /* LLVMStructType_h */