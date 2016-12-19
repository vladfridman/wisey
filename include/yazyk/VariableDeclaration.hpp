//
//  VariableDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef VariableDeclaration_h
#define VariableDeclaration_h

#include "yazyk/node.hpp"

namespace yazyk {
  
/**
 * Represents a variable declaration
 */
class VariableDeclaration : public IStatement {
  const TypeSpecifier& mType;
  Identifier& mId;
  IExpression *mAssignmentExpression;

public:
  VariableDeclaration(const TypeSpecifier& type, Identifier& id) :
    mType(type), mId(id), mAssignmentExpression(NULL) { }

  VariableDeclaration(const TypeSpecifier& type,
                      Identifier& id,
                      IExpression *assignmentExpression) :
    mType(type), mId(id), mAssignmentExpression(assignmentExpression) { }

  ~VariableDeclaration() { }
  
  const TypeSpecifier& getType() const;
  
  const Identifier& getId() const;
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};

} /* namespace yazyk */

#endif /* VariableDeclaration_h */
