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
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {

/**
 * Represents a concrete object's method declaration.
 *
 * A method contains a coumpound statement that is the body of the method.
 */
class MethodDeclaration : public IMethodDeclaration {
  const AccessLevel mAccessLevel;
  const ITypeSpecifier* mReturnTypeSpecifier;
  std::string mName;
  VariableList mArguments;
  std::vector<IModelTypeSpecifier*> mExceptions;
  CompoundStatement* mCompoundStatement;
  int mLine;
  
public:
  
  MethodDeclaration(const AccessLevel AccessLevel,
                    const ITypeSpecifier* returnTypeSpecifier,
                    std::string name,
                    VariableList arguments,
                    std::vector<IModelTypeSpecifier*> exceptions,
                    CompoundStatement* compoundStatement,
                    int line);
  
  ~MethodDeclaration();
  
  IMethod* declare(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* MethodDeclaration_h */
