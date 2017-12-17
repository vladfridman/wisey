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
class ArrayElementType;
  
/**
 * Lists possible data type kinds
 */
typedef enum TypeKindEnum {
  PRIMITIVE_TYPE,
  ARRAY_TYPE,
  ARRAY_ELEMENT_TYPE,
  FUNCTION_TYPE,
  PACKAGE_TYPE,
  INTERFACE_TYPE,
  INTERFACE_OWNER_TYPE,
  MODEL_TYPE,
  MODEL_OWNER_TYPE,
  CONTROLLER_TYPE,
  CONTROLLER_OWNER_TYPE,
  NODE_TYPE,
  NODE_OWNER_TYPE,
  NULL_TYPE_KIND,
  UNDEFINED_TYPE_KIND,
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
  virtual std::string getTypeName() const = 0;
  
  /**
   * Return corresponding LLVM type
   */
  virtual llvm::Type* getLLVMType(IRGenerationContext& context) const = 0;
  
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
                              const IType* toType,
                              int line) const = 0;
  
  /**
   * Returns an ArrayElementType with the base type as this type
   */
  virtual const ArrayElementType* getArrayElementType() const = 0;
  
  /**
   * Tells whether the given type is an owner type
   */
  static bool isOwnerType(const IType* type);
  
  /**
   * Tells whether the given type is a reference type
   */
  static bool isReferenceType(const IType* type);
  
  /**
   * Tells whether the given type is a concrete object type such as model, node or controller
   */
  static bool isConcreteObjectType(const IType* type);
  
};

} /* namespace wisey */

#endif /* IType_h */
