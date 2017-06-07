//
//  IType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IType_h
#define IType_h

#include <string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace wisey {

class IRGenerationContext;
  
/**
 * Lists possible data type kinds
 */
typedef enum TypeKindEnum {
  PRIMITIVE_TYPE,
  INTERFACE_TYPE,
  MODEL_TYPE,
  CONTROLLER_TYPE,
} TypeKind;

/**
 * Interface representing expression type in wisey language
 */
class IType {
  
public:
  
  virtual ~IType() { }
  
  /**
   * Returns type name
   */
  virtual std::string getName() const = 0;
  
  /**
   * Return corresponding LLVM type
   */
  virtual llvm::Type* getLLVMType(llvm::LLVMContext& llvmContext) const = 0;
  
  /**
   * Tells what kind of data type is this type: primitive/model/controller
   */
  virtual TypeKind getTypeKind() const = 0;
  
  /**
   * Indicates whether this type can be cast to another type
   */
  virtual bool canCastTo(const IType* toType) const = 0;

  /**
   * Indicates whether this type can be auto cast to another type
   */
  virtual bool canAutoCastTo(const IType* toType) const = 0;
  
  /**
   * Casts given value to a given type
   */
  virtual llvm::Value* castTo(IRGenerationContext& context,
                              llvm::Value* fromValue,
                              const IType* toType) const = 0;
};

} /* namespace wisey */

#endif /* IType_h */
