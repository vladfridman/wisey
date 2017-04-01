//
//  MethodSignatureDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignatureDeclaration_h
#define MethodSignatureDeclaration_h

#include "yazyk/AccessSpecifier.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/ITypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

namespace yazyk {
  
/**
 * Represents a method signature declaration contained within an interface
 */
class MethodSignatureDeclaration {
  const ITypeSpecifier& mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  
public:
  
  MethodSignatureDeclaration(const ITypeSpecifier& returnTypeSpecifier,
                  std::string methodName,
                  const VariableList& arguments) :
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments) { }
  
  ~MethodSignatureDeclaration() { };
  
  /**
   * Returns object representing the method for storage in an Interface object
   */
  MethodSignature* createMethodSignature(IRGenerationContext& context, unsigned long index) const;

};
  
} /* namespace yazyk */

#endif /* MethodSignatureDeclaration_h */
