//
//  StaticMethodDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StaticMethodDeclaration_h
#define StaticMethodDeclaration_h

#include "wisey/AccessLevel.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {

/**
 * Represents a concrete object's static method declaration.
 *
 * Static method does not have access to object instance
 */
class StaticMethodDeclaration : public IMethodDeclaration {
  const AccessLevel mAccessLevel;
  const ITypeSpecifier* mReturnTypeSpecifier;
  std::string mName;
  VariableList mArguments;
  std::vector<ModelTypeSpecifier*> mExceptions;
  CompoundStatement* mCompoundStatement;
  
public:
  
  StaticMethodDeclaration(const AccessLevel AccessLevel,
                          const ITypeSpecifier* returnTypeSpecifier,
                          std::string name,
                          VariableList arguments,
                          std::vector<ModelTypeSpecifier*> exceptions,
                          CompoundStatement* compoundStatement);
  
  ~StaticMethodDeclaration();
  
  IMethod* declare(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* StaticMethodDeclaration_h */
