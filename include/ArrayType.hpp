//
//  ArrayType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayType_h
#define ArrayType_h

#include <llvm/IR/Instructions.h>

#include "ArrayOwnerType.hpp"
#include "IReferenceType.hpp"
#include "IType.hpp"

namespace wisey {
  
  class ImmutableArrayType;
  
  /**
   * Represents the array type
   *
   * Arrays are wrapped in a Struct in order to keep track of reference count and array size
   * The Struct looks as follows:
   * {
   *   int_64 ref_count
   *   int_64 array_size
   *   int_64 element_size_in_bytes
   *   [ array ]
   * }
   */
  class ArrayType : public IReferenceType {
    
    const IType* mElementType;
    unsigned long mNumberOfDimensions;
    const ArrayOwnerType* mArrayOwnerType;
    const ImmutableArrayType* mImmutableArrayType;
    
  public:
    
    ArrayType(const IType* elementType, unsigned long numberOfDimensions);
    
    ~ArrayType();
    
    /**
     * Index of the actual elements array in the struct that represents a wisey array
     */
    static const unsigned int ARRAY_ELEMENTS_START_INDEX;
    
    /**
     * Returns the owner type for this array type
     */
    const ArrayOwnerType* getOwner() const override;
    
    /**
     * Returns the number of dimensions in this array
     */
    unsigned long getNumberOfDimensions() const;
    
    /**
     * Returns single array element type
     */
    const IType* getElementType() const;
    
    /**
     * Returns an immutable array type for this array type
     */
    const ImmutableArrayType* getImmutable() const;
    
    /**
     * Given a pointer to the array structure returns a pointer to the llvm array within it
     */
    static llvm::Value* extractLLVMArray(IRGenerationContext& context,
                                         llvm::Value* arrayPointer);
    
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

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;
    
  private:
    
    llvm::Value* bitcastToGenericPointer(IRGenerationContext& context,
                                         llvm::Value* arrayPointer) const;
    
  };
  
} /* namespace wisey */

#endif /* ArrayType_h */

