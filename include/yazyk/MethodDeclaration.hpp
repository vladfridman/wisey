//
//  MethodDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef MethodDeclaration_h
#define MethodDeclaration_h

#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/CompoundStatement.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/ITypeSpecifier.hpp"
#include "yazyk/Method.hpp"
#include "yazyk/VariableDeclaration.hpp"

namespace yazyk {

/**
 * Represents a function declaration.
 *
 * A function contains Block that contains statements.
 */
class MethodDeclaration : public IStatement {
  const AccessSpecifier mAccessSpecifier;
  const ITypeSpecifier& mTypeSpecifier;
  const Identifier& mId;
  VariableList mArguments;
  CompoundStatement& mCompoundStatement;
  
public:
  MethodDeclaration(const AccessSpecifier& accessSpecifier,
                    const ITypeSpecifier& typeSpecifier,
                    const Identifier& id,
                    const VariableList& arguments,
                    CompoundStatement& compoundStatement) :
  mAccessSpecifier(accessSpecifier),
  mTypeSpecifier(typeSpecifier),
  mId(id),
  mArguments(arguments),
  mCompoundStatement(compoundStatement) { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

  /**
   * Returns object representing the method for storage in a model object
   */
  Method* getMethod(IRGenerationContext& context) const;
};

} /* namespace yazyk */

#endif /* MethodDeclaration_h */
