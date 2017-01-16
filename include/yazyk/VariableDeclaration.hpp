//
//  VariableDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef VariableDeclaration_h
#define VariableDeclaration_h

#include "yazyk/IStatement.hpp"
#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {
  
/**
 * Represents a variable declaration
 */
class VariableDeclaration : public IStatement {
  const ITypeSpecifier& mType;
  Identifier& mId;
  IExpression *mAssignmentExpression;

public:
  VariableDeclaration(const ITypeSpecifier& type, Identifier& id) :
    mType(type), mId(id), mAssignmentExpression(NULL) { }

  VariableDeclaration(const ITypeSpecifier& type,
                      Identifier& id,
                      IExpression *assignmentExpression) :
    mType(type), mId(id), mAssignmentExpression(assignmentExpression) { }

  ~VariableDeclaration() { }
  
  const ITypeSpecifier& getType() const;
  
  const Identifier& getId() const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

/**
 * Represents a list of variables
 */
typedef std::vector<VariableDeclaration*> VariableList;

} /* namespace yazyk */

#endif /* VariableDeclaration_h */
