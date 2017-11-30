//
//  ControllerTypeSpecifierFull.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerTypeSpecifierFull_h
#define ControllerTypeSpecifierFull_h

#include "wisey/IControllerTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a fully qualified controller type specifier
 */
class ControllerTypeSpecifierFull : public IControllerTypeSpecifier {
  const std::string mPackage;
  const std::string mShortName;

public:
  
  ControllerTypeSpecifierFull(std::string package, std::string shortName);
  
  ~ControllerTypeSpecifierFull();
  
  std::string getShortName() const override;
  
  std::string getPackage() const override;

  std::string getName(IRGenerationContext& context) const override;
  
  const Controller* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* ControllerTypeSpecifierFull_h */
