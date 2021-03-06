//
//  ArrayExactType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayExactType_h
#define ArrayExactType_h

#include <list>

#include <llvm/IR/DerivedTypes.h>

#include "ArrayType.hpp"
#include "IType.hpp"

namespace wisey {
  
  /**
   * Represents the array type where each dimension is exactly defined and has a constant value.
   *
   * This class is used for static array allocation command e.g. { 1, 2, 3, 4 };
   */
  class ArrayExactType : public IType {
    
    const IType* mElementType;
    std::list<unsigned long> mDimensions;
    
  public:
    
    ArrayExactType(const IType* elementType, std::list<unsigned long> dimensions);
    
    ~ArrayExactType();
    
    /**
     * Returns the number of dimensions in this array
     */
    unsigned long getNumberOfDimensions() const;
    
    /**
     * Return array dimensions
     */
    std::list<unsigned long> getDimensions() const;
    
    /**
     * Returns single array element type
     */
    const IType* getElementType() const;
    
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

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;

  private:
    
    llvm::Value* allocateArray(IRGenerationContext& context,
                               llvm::Value* staticArray,
                               const IType* toType) const;
    
  };
  
} /* namespace wisey */

#endif /* ArrayExactType_h */

