//
//  ControllerTypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerTypeSpecifier_h
#define ControllerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"

namespace yazyk {
  
/**
 * Represents CONTROLLER type specifier
 */
class ControllerTypeSpecifier : public ITypeSpecifier {
  const std::string mName;
  
public:
  
  ControllerTypeSpecifier(std::string name) : mName(name) { }
  
  ~ControllerTypeSpecifier() { }
  
  const std::string getName() const;
  
  IType* getType(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* ControllerTypeSpecifier_h */
