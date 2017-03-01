//
//  MethodSignature.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignature_h
#define MethodSignature_h

#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/ITypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

namespace yazyk {
  
/**
 * Represents a method signature contained within an interface
 */
class MethodSignature {
  const AccessSpecifier mAccessSpecifier;
  const ITypeSpecifier& mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  
public:
  
  MethodSignature(const AccessSpecifier& accessSpecifier,
                  const ITypeSpecifier& returnTypeSpecifier,
                  std::string methodName,
                  const VariableList& arguments) :
  mAccessSpecifier(accessSpecifier),
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments) { }
  
  ~MethodSignature() { };
  
  /**
   * Returns object representing the method for storage in an Interface object
   */
  Method* getMethod(IRGenerationContext& context) const;

};
  
} /* namespace yazyk */

#endif /* MethodSignature_h */
