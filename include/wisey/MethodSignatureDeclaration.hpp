//
//  MethodSignatureDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignatureDeclaration_h
#define MethodSignatureDeclaration_h

#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalMethod.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
/**
 * Represents a method signature declaration contained within an interface
 */
class MethodSignatureDeclaration : public IObjectElementDeclaration {
  const ITypeSpecifier* mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  std::vector<ModelTypeSpecifier*> mThrownExceptions;
  
public:
  
  MethodSignatureDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                             std::string methodName,
                             const VariableList& arguments,
                             std::vector<ModelTypeSpecifier*> thrownExceptions) :
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions) { }
  
  ~MethodSignatureDeclaration();
  
  MethodSignature* declare(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* MethodSignatureDeclaration_h */
