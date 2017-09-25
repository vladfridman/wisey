//
//  Assignment.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Assignment_h
#define Assignment_h

#include "wisey/Identifier.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"

namespace wisey {
  
/**
 * Represents an assignment expression
 */
class Assignment : public IExpression {
  Identifier* mIdentifier;
  IExpression* mExpression;
  
public:

  Assignment(Identifier* identifier, IExpression* expression)
    : mIdentifier(identifier), mExpression(expression) { }

  ~Assignment();
  
  IVariable* getVariable(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  void printToStream(std::iostream& stream) const override;

private:
  
  llvm::Value* generateIRForHeapVariable(IRGenerationContext& context) const;
  
  llvm::Value* generateIRForStackVariable(IRGenerationContext& context) const;
};
  
}

#endif /* Assignment_h */
