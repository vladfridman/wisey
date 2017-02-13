//
//  IVariable.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IVariable_h
#define IVariable_h

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

#include "yazyk/IType.hpp"

namespace yazyk {
  
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
  virtual IType* getType() const = 0;

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
                                            IExpression& assignToExpression) = 0;
  
  /**
   * Free any allocated space associated with this variable
   */
  virtual void free(llvm::BasicBlock* basicBlock) const = 0;
};
  
} /* namespace yazyk */

#endif /* IVariable_h */