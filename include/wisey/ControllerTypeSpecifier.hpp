//
//  ControllerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerTypeSpecifier_h
#define ControllerTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents controller type specifier
 */
class ControllerTypeSpecifier : public IObjectTypeSpecifier {
  const std::vector<std::string> mPackage;
  const std::string mName;
  
public:
  
  ControllerTypeSpecifier(std::vector<std::string> package, std::string name)
  : mPackage(package), mName(name) { }
  
  ~ControllerTypeSpecifier() { }
  
  Controller* getType(IRGenerationContext& context) const override;
  
  void printToStream(std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* ControllerTypeSpecifier_h */
