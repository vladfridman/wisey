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
#include "wisey/Identifier.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

namespace wisey {
  
/**
 * Represents a method signature declaration contained within an interface
 */
class MethodSignatureDeclaration {
  const ITypeSpecifier& mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  std::vector<ITypeSpecifier*> mThrownExceptions;
  
public:
  
  MethodSignatureDeclaration(const ITypeSpecifier& returnTypeSpecifier,
                             std::string methodName,
                             const VariableList& arguments,
                             std::vector<ITypeSpecifier*> thrownExceptions) :
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions) { }
  
  ~MethodSignatureDeclaration() { };
  
  /**
   * Returns object representing the method for storage in an Interface object
   */
  MethodSignature* createMethodSignature(IRGenerationContext& context, unsigned long index) const;

};
  
} /* namespace wisey */

#endif /* MethodSignatureDeclaration_h */