//
//  VariableDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef VariableDeclaration_h
#define VariableDeclaration_h

#include "wisey/Identifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a variable declaration
 */
class VariableDeclaration : public IStatement {
  const ITypeSpecifier* mTypeSpecifier;
  Identifier* mId;
  IExpression* mAssignmentExpression;

public:
  VariableDeclaration(const ITypeSpecifier* typeSpecifier, Identifier* id) :
    mTypeSpecifier(typeSpecifier), mId(id), mAssignmentExpression(NULL) { }

  VariableDeclaration(const ITypeSpecifier* typeSpecifier,
                      Identifier* id,
                      IExpression* assignmentExpression) :
    mTypeSpecifier(typeSpecifier), mId(id), mAssignmentExpression(assignmentExpression) { }

  ~VariableDeclaration();
  
  const ITypeSpecifier* getTypeSpecifier() const;
  
  const Identifier* getId() const;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  void allocateOnStack(IRGenerationContext& context) const;
  
  void allocateOwnerOnHeap(IRGenerationContext& context) const;
  
  void allocateReferenceOnHeap(IRGenerationContext& context) const;
};

/**
 * Represents a list of variables
 */
typedef std::vector<VariableDeclaration*> VariableList;

} /* namespace wisey */

#endif /* VariableDeclaration_h */
