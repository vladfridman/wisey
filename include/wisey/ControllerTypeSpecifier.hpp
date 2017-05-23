//
//  ControllerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerTypeSpecifier_h
#define ControllerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents CONTROLLER type specifier
 */
class ControllerTypeSpecifier : public ITypeSpecifier {
  const std::string mName;
  
public:
  
  ControllerTypeSpecifier(std::string name) : mName(name) { }
  
  ~ControllerTypeSpecifier() { }
  
  IType* getType(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* ControllerTypeSpecifier_h */
