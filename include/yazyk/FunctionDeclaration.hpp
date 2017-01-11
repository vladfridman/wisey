//
//  FunctionDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef FunctionDeclaration_h
#define FunctionDeclaration_h

#include "yazyk/CompoundStatement.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/TypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

namespace yazyk {

/**
 * Represents a function declaration.
 *
 * A function contains Block that contains statements.
 */
class FunctionDeclaration : public IStatement {
  const TypeSpecifier& mType;
  const Identifier& mId;
  VariableList mArguments;
  CompoundStatement& mCompoundStatement;
  
public:
  FunctionDeclaration(const TypeSpecifier& type,
                      const Identifier& id,
                      const VariableList& arguments,
                      CompoundStatement& compoundStatement) :
  mType(type), mId(id), mArguments(arguments), mCompoundStatement(compoundStatement) { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* FunctionDeclaration_h */
