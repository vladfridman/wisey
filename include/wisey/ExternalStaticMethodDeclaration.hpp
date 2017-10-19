//
//  ExternalStaticMethodDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalStaticMethodDeclaration_h
#define ExternalStaticMethodDeclaration_h

#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
/**
 * Represents a static method declaration with an external implemention
 */
class ExternalStaticMethodDeclaration : public IMethodDeclaration {
  const ITypeSpecifier* mReturnTypeSpecifier;
  std::string mName;
  VariableList mArguments;
  std::vector<ModelTypeSpecifier*> mThrownExceptions;
  
public:
  
  ExternalStaticMethodDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                            std::string name,
                            const VariableList& arguments,
                            std::vector<ModelTypeSpecifier*> thrownExceptions) :
  mReturnTypeSpecifier(returnTypeSpecifier),
  mName(name),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions) { }
  
  ~ExternalStaticMethodDeclaration();
  
  ExternalStaticMethod* declare(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* ExternalStaticMethodDeclaration_h */
