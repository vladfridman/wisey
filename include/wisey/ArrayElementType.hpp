//
//  ArrayElementType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayElementType_h
#define ArrayElementType_h

#include "wisey/IType.hpp"

namespace wisey {
  
  /**
   * Represents a pointer that points to an array element.
   *
   * If we have an int[20] array then its element array[0] could either be used to evaluate
   * expression value such as
   *
   * int a = array[0];
   *
   * or it could be assigned to such as
   *
   * array[0] = 0;
   *
   * The ArrayElementType is needed to satisfy this two scenarios. In the first case
   * ArrayElementType with base type int will be auto cast to int, in the other the value will
   * be stored in the expression on the left which will be a pointer to int in LLVM
   */
  class ArrayElementType : public IType {
    
    const IType* mBaseType;
    
  public:
    
    ArrayElementType(const IType* baseType);
    
    ~ArrayElementType();
    
    /**
     * Return array base type
     */
    const IType* getBaseType() const;
    
    std::string getTypeName() const override;
    
    llvm::Type* getLLVMType(IRGenerationContext& context) const override;
    
    TypeKind getTypeKind() const override;
    
    bool canCastTo(const IType* toType) const override;
    
    bool canAutoCastTo(const IType* toType) const override;
    
    llvm::Value* castTo(IRGenerationContext& context,
                        llvm::Value* fromValue,
                        const IType* toType,
                        int line) const override;
    
    ArrayElementType* getArrayElementType() const override;
    
  };
  
}

#endif /* ArrayElementType_h */
