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
 * They could be allocated at different levels: model or local
 * The could be used in situations such as assignment or reference
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
   * Returns variable's value
   */
  virtual llvm::Value* getValue() const = 0;

  /**
   * Generated LLVM IR for the variable when it used in an identifier
   */
  virtual llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                            std::string llvmVariableName) const = 0;

  /**
   * Generate LLVM IR for when it is assigned to a given value
   */
  virtual llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                            IExpression* assignToExpression) = 0;
  
  /**
   * Free any allocated space associated with this variable
   */
  virtual void free(IRGenerationContext& context) const = 0;
  
  /**
   * Tells whether this variable exists in the scope outside the method where this variable is used
   */
  virtual bool existsInOuterScope() const = 0;

  /**
   * Creates a name for naming temporary heap variables in the scope
   */
  static std::string getTemporaryVariableName(const void* object);

};
  
} /* namespace wisey */

#endif /* IVariable_h */
