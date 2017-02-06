//
//  ITypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ITypeSpecifier_h
#define ITypeSpecifier_h

#include "yazyk/IType.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Variable.hpp"

namespace yazyk {

/**
 * Interfaces representing a type that could either be a primitive type, a model or a controller
 */
class ITypeSpecifier {
  
public:
  
  virtual ~ITypeSpecifier() { }
  
  /**
   * Get the type corresponding to the type specifier
   */
  virtual IType* getType(IRGenerationContext& context) const = 0;
  
  /**
   * Tells whether this type should be allocated on stack or heap
   */
  virtual VariableStorageType getStorageType() const = 0;
};

} /* namespace yazyk */

#endif /* ITypeSpecifier_h */
