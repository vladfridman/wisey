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

#include "wisey/IPrintable.hpp"

namespace wisey {

  class ArrayType;
  class IConcreteObjectType;
  class IRGenerationContext;

  /**
   * Lists possible data type kinds
   */
  typedef enum TypeKindEnum {
    PRIMITIVE_TYPE,
    ARRAY_TYPE,
    ARRAY_OWNER_TYPE,
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
  class IType : public IPrintable {
    
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
    virtual bool canCastTo(IRGenerationContext& context, const IType* toType) const = 0;
    
    /**
     * Indicates whether this type can be auto cast to another type
     */
    virtual bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const = 0;
    
    /**
     * Casts given value to a given type
     */
    virtual llvm::Value* castTo(IRGenerationContext& context,
                                llvm::Value* fromValue,
                                const IType* toType,
                                int line) const = 0;
    
    /**
     * Tells whether this type is an owner pointer type
     */
    virtual bool isOwner() const = 0;
    
    /**
     * Tells whether this type is a reference pointer type
     */
    virtual bool isReference() const = 0;
    
    /**
     * Tells whether this type is of array type
     */
    virtual bool isArray() const = 0;
    
    /**
     * Tells whether this type is of controller type
     */
    virtual bool isController() const = 0;
    
    /**
     * Tells whether this type is of interface type
     */
    virtual bool isInterface() const = 0;
    
    /**
     * Tells whether this type is of model type
     */
    virtual bool isModel() const = 0;
    
    /**
     * Tells whether this type is of node type
     */
    virtual bool isNode() const = 0;

    /**
     * Allocates local variable of the type and stores it in the scope
     */
    virtual void createLocalVariable(IRGenerationContext& context, std::string name) const = 0;

    /**
     * Allocates field variable of the type and stores it in the scope
     */
    virtual void createFieldVariable(IRGenerationContext& context,
                                     std::string name,
                                     const IConcreteObjectType* object) const = 0;

    /**
     * Allocates parameter variable of the type and stores it in the scope
     */
    virtual void createParameterVariable(IRGenerationContext& context,
                                         std::string name,
                                         llvm::Value* value) const = 0;

    /**
     * Returns ArrayType if this type has one associated with it otherwise reports an error
     */
    virtual const ArrayType* getArrayType(IRGenerationContext& context) const = 0;
    
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
        
    /**
     * Tells whether the given type is a primitive type
     */
    static bool isPrimitveType(const IType* type);
    
  };

} /* namespace wisey */

#endif /* IType_h */
