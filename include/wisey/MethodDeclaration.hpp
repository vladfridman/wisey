//
//  MethodDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef MethodDeclaration_h
#define MethodDeclaration_h

#include "wisey/AccessLevel.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/Model.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {

/**
 * Represents a model's method declaration.
 *
 * A method contains a coumpound statement that is the body of the method.
 */
class MethodDeclaration {
  const AccessLevel mAccessLevel;
  const ITypeSpecifier& mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  std::vector<ITypeSpecifier*> mExceptions;
  CompoundStatement& mCompoundStatement;
  
public:
  
  MethodDeclaration(const AccessLevel AccessLevel,
                    const ITypeSpecifier& returnTypeSpecifier,
                    std::string methodName,
                    const VariableList& arguments,
                    std::vector<ITypeSpecifier*> exceptions,
                    CompoundStatement& compoundStatement) :
  mAccessLevel(AccessLevel),
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments),
  mExceptions(exceptions),
  mCompoundStatement(compoundStatement) { }
  
  /**
   * Returns object representing the method that will be saved in a Model object
   */
  Method* createMethod(IRGenerationContext& context, unsigned long index) const;
  
  /**
   * Returns method's name
   */
  std::string getMethodName() const;
  
};

} /* namespace wisey */

#endif /* MethodDeclaration_h */
