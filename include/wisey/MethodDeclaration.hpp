//
//  MethodDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef MethodDeclaration_h
#define MethodDeclaration_h

#include "wisey/AccessLevel.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {

/**
 * Represents a model's method declaration.
 *
 * A method contains a coumpound statement that is the body of the method.
 */
class MethodDeclaration : public IMethodDeclaration {
  const AccessLevel mAccessLevel;
  const ITypeSpecifier* mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  std::vector<ModelTypeSpecifier*> mExceptions;
  CompoundStatement* mCompoundStatement;
  
public:
  
  MethodDeclaration(const AccessLevel AccessLevel,
                    const ITypeSpecifier* returnTypeSpecifier,
                    std::string methodName,
                    VariableList arguments,
                    std::vector<ModelTypeSpecifier*> exceptions,
                    CompoundStatement* compoundStatement) :
  mAccessLevel(AccessLevel),
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments),
  mExceptions(exceptions),
  mCompoundStatement(compoundStatement) { }
  
  ~MethodDeclaration();
  
  Method* createMethod(IRGenerationContext& context, unsigned long index) const override;
  
};

} /* namespace wisey */

#endif /* MethodDeclaration_h */
