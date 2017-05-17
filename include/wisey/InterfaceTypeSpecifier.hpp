
//
//  InterfaceTypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceTypeSpecifier_h
#define InterfaceTypeSpecifier_h

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents Interface type specifier
 */
class InterfaceTypeSpecifier : public ITypeSpecifier {
  const std::string mName;
  
public:
  
  InterfaceTypeSpecifier(std::string name) : mName(name) { }
  
  ~InterfaceTypeSpecifier() { }
  
  const std::string getName() const;
  
  IType* getType(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* InterfaceTypeSpecifier_h */
