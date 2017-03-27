//
//  ControllerFieldDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerFieldDeclaration_h
#define ControllerFieldDeclaration_h

#include "yazyk/IExpression.hpp"
#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {
  
/**
 * Lists possible controller field qualifiers
 */
typedef enum FieldQualifierEnum {
  RECEIVED_FIELD,
  INJECTED_FIELD,
  STATE_FIELD,
} FieldQualifier;
  
/**
 * Represents a field in CONTROLLER definition
 */
class ControllerFieldDeclaration {
  
  FieldQualifier mFieldQualifier;
  ITypeSpecifier& mTypeSpecifier;
  std::string mName;
  ExpressionList& mArguments;
  
public:
  
  ControllerFieldDeclaration(FieldQualifier fieldQualifier,
                             ITypeSpecifier& typeSpecifier,
                             std::string name,
                             ExpressionList& arguments) :
  mFieldQualifier(fieldQualifier),
  mTypeSpecifier(typeSpecifier),
  mName(name),
  mArguments(arguments) { }
  
  ~ControllerFieldDeclaration() { }
  
  FieldQualifier getFieldQualifier() const;
  
  ITypeSpecifier& getTypeSpecifier() const;
  
  std::string getName() const;
};
  
} /* namespace yazyk */

#endif /* ControllerFieldDeclaration_h */
