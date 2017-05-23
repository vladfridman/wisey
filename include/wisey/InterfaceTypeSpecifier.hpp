
//
//  InterfaceTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceTypeSpecifier_h
#define InterfaceTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents Interface type specifier
 */
class InterfaceTypeSpecifier : public ITypeSpecifier {
  const std::vector<std::string> mPackage;
  const std::string mName;
  
public:
  
  InterfaceTypeSpecifier(std::vector<std::string> package, std::string name)
  : mPackage(package), mName(name) { }
  
  ~InterfaceTypeSpecifier() { }
  
  IType* getType(IRGenerationContext& context) const override;
};
  
} /* namespace wisey */

#endif /* InterfaceTypeSpecifier_h */
