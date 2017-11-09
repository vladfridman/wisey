//
//  ParameterReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ParameterReferenceVariable_h
#define ParameterReferenceVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
/**
 * Represents a method parameter that is a reference to an object owner.
 *
 * This is the same as LocalReferenceVariable only returns true on existsInOuterScope()
 */
class ParameterReferenceVariable : public IReferenceVariable {
  
  std::string mName;
  const IObjectType* mType;
  llvm::Value* mValue;
  
public:
  
  ParameterReferenceVariable(std::string name, const IObjectType* type, llvm::Value* value);
  
  ~ParameterReferenceVariable();
  
  std::string getName() const override;
  
  const IObjectType* getType() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;
  
  bool existsInOuterScope() const override;
  
  void decrementReferenceCounter(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* ParameterReferenceVariable_h */
