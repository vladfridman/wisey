
//
//  InterfaceTypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceTypeSpecifier_h
#define InterfaceTypeSpecifier_h

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IType.hpp"
#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {
  
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
  
} /* namespace yazyk */

#endif /* InterfaceTypeSpecifier_h */
