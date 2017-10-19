//
//  ExternalMethodDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalMethodDeclaration_h
#define ExternalMethodDeclaration_h

#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalMethod.hpp"
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
/**
 * Represents method declaration for an externally implemented object
 */
class ExternalMethodDeclaration : public IMethodDeclaration {
  const ITypeSpecifier* mReturnTypeSpecifier;
  std::string mName;
  VariableList mArguments;
  std::vector<ModelTypeSpecifier*> mThrownExceptions;
  
public:
  
  ExternalMethodDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                            std::string name,
                            const VariableList& arguments,
                            std::vector<ModelTypeSpecifier*> thrownExceptions) :
  mReturnTypeSpecifier(returnTypeSpecifier),
  mName(name),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions) { }
  
  ~ExternalMethodDeclaration();
  
  ExternalMethod* declare(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* ExternalMethodDeclaration_h */
