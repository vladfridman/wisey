//
//  FieldDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldDeclaration_h
#define FieldDeclaration_h

#include "wisey/IExpression.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Lists possible controller field qualifiers
 */
typedef enum FieldQualifierEnum {
  FIXED_FIELD,
  RECEIVED_FIELD,
  INJECTED_FIELD,
  STATE_FIELD,
} FieldQualifier;
  
/**
 * Represents a field in CONTROLLER definition
 */
class FieldDeclaration {
  
  FieldQualifier mFieldQualifier;
  ITypeSpecifier* mTypeSpecifier;
  std::string mName;
  ExpressionList mArguments;
  
public:
  
  FieldDeclaration(FieldQualifier fieldQualifier,
                             ITypeSpecifier* typeSpecifier,
                             std::string name,
                             ExpressionList arguments) :
  mFieldQualifier(fieldQualifier),
  mTypeSpecifier(typeSpecifier),
  mName(name),
  mArguments(arguments) { }
  
  ~FieldDeclaration();
  
  FieldQualifier getFieldQualifier() const;
  
  ITypeSpecifier* getTypeSpecifier() const;
  
  std::string getName() const;
  
  ExpressionList getArguments() const;
};
  
} /* namespace wisey */

#endif /* FieldDeclaration_h */
