//
//  FieldDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldDeclaration_h
#define FieldDeclaration_h

#include "wisey/FieldKind.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {

class Field;
  
/**
 * Represents a field in controller definition
 */
class FieldDeclaration : public IObjectElementDeclaration {
  FieldKind mFieldKind;
  ITypeSpecifier* mTypeSpecifier;
  std::string mName;
  InjectionArgumentList mInjectionArgumentList;
  
public:
  
  FieldDeclaration(FieldKind fieldKind,
                   ITypeSpecifier* typeSpecifier,
                   std::string name,
                   InjectionArgumentList injectionArguments);
  
  ~FieldDeclaration();
  
  Field* declare(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* FieldDeclaration_h */
