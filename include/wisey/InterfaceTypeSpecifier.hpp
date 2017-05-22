
//
//  InterfaceTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceTypeSpecifier_h
#define InterfaceTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents Interface type specifier
 */
class InterfaceTypeSpecifier : public IObjectTypeSpecifier {
  const std::string mName;
  
public:
  
  InterfaceTypeSpecifier(std::string name) : mName(name) { }
  
  ~InterfaceTypeSpecifier() { }
  
  const std::string getName() const override;
  
  IType* getType(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* InterfaceTypeSpecifier_h */
