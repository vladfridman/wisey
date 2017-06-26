//
//  ModelTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelTypeSpecifier_h
#define ModelTypeSpecifier_h

#include <string>

#include "wisey/IBuildableConcreteObjectTypeSpecifier.hpp"

namespace wisey {

/**
 * Represents model type specifier
 */
class ModelTypeSpecifier : public IBuildableConcreteObjectTypeSpecifier {
  const std::vector<std::string> mPackage;
  const std::string mName;

public:
  
  ModelTypeSpecifier(std::vector<std::string> package, std::string name)
  : mPackage(package), mName(name) { }
  
  ~ModelTypeSpecifier() { }

  Model* getType(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* ModelTypeSpecifier_h */
