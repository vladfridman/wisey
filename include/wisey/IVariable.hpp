//
//  IVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IVariable_h
#define IVariable_h

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

#include "wisey/IType.hpp"

namespace wisey {
  
class IRGenerationContext;
class IExpression;

/**
 * Interface representing all variables
 *
 * Variables could have a different storage type: stack or heap
 * They could be allocated at different levels: object or local
 * The could be used in situations such as assignment or reference
 *
 * Primitve variables are pretty simple to deal with. It is more diffucult to deal with object
 * type variables of which there are two kinds: owner and reference.
 *
 * An owner variable is a pointer to a struct where the struct represents the object.
 * Owner variable is stored somewhere. If owner variable is local then it is stored in on a stack
 * allocated memory that was allocated using alloc command. If an owner variable is stored in an
 * object then it is stored in a field that has a type Struct*. Either way the variable that
 * represents an object owner has type Struct**.
 *
 * Reference variables refer to an object owner. Thus local reference would be a pointer to owner.
 * This translates into a type Struct**. The local reference is stored in a variable created using
 * alloc command. That variable has type Struct***.
 *
 */
class IVariable {
  
public:
  
  virtual ~IVariable() { }
  
  /**
   * Returns variable's name
   */
  virtual std::string getName() const = 0;

  /**
   * Returns variable's type
   */
  virtual const IType* getType() const = 0;

  /**
   * Generated LLVM IR for the variable when it used in an identifier
   */
  virtual llvm::Value* generateIdentifierIR(IRGenerationContext& context) const = 0;

  /**
   * Generate LLVM IR for when it is assigned to a given value
   */
  virtual llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                            IExpression* assignToExpression) = 0;
  
  /**
   * Tells whether this variable exists in the scope outside the method where this variable is used
   */
  virtual bool existsInOuterScope() const = 0;

  /**
   * Creates a name for naming temporary heap variables in the scope
   */
  static std::string getTemporaryVariableName(const void* object);

  /**
   * Get variable reference from scopes and print an error message if the variable is missing
   */
  static IVariable* getVariable(IRGenerationContext& context, std::string name);
  
};
  
} /* namespace wisey */

#endif /* IVariable_h */
