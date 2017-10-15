
//
//  InterfaceTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceTypeSpecifier_h
#define InterfaceTypeSpecifier_h

#include "wisey/IInjectableObjectTypeSpecifier.hpp"
#include "wisey/IInjectable.hpp"
#include "wisey/Interface.hpp"

namespace wisey {
  
/**
 * Represents interface type specifier
 */
class InterfaceTypeSpecifier : public IInjectableObjectTypeSpecifier {
  const std::vector<std::string> mPackage;
  const std::string mShortName;
  
public:
  
  InterfaceTypeSpecifier(std::vector<std::string> package, std::string shortName)
  : mPackage(package), mShortName(shortName) { }
  
  ~InterfaceTypeSpecifier() { }
  
  std::string getShortName() const override;
  
  std::string getName(IRGenerationContext& context) const override;
  
  Interface* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* InterfaceTypeSpecifier_h */
