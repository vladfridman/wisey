//
//  ImmutableArrayType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ImmutableArrayType_h
#define ImmutableArrayType_h

#include <llvm/IR/Instructions.h>

#include "wisey/ArrayType.hpp"
#include "wisey/IReferenceType.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"

namespace wisey {
  
  /**
   * Represents the immutable array type: an immutable array can not be modified
   */
  class ImmutableArrayType : public IReferenceType {
    
    const ArrayType* mArrayType;
    const ImmutableArrayOwnerType* mImmutableArrayOwnerType;
    
  public:
    
    ImmutableArrayType(const ArrayType* arrayType);
    
    ~ImmutableArrayType();
    
    /**
     * Returns the owner type for this immutable array type
     */
    const ImmutableArrayOwnerType* getOwner() const override;
    
    /**
     * Returns the number of dimensions in this array
     */
    unsigned long getNumberOfDimensions() const;
    
    /**
     * Returns single array element type
     */
    const IType* getElementType() const;
    
    void incrementReferenceCount(IRGenerationContext& context,
                                 llvm::Value* arrayPointer) const override;
    
    void decrementReferenceCount(IRGenerationContext& context,
                                 llvm::Value* arrayPointer) const override;
    
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
    
    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;
    
  };
  
} /* namespace wisey */

#endif /* ImmutableArrayType_h */
