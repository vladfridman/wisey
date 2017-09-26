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
  const std::string mShortName;

public:
  
  ModelTypeSpecifier(std::vector<std::string> package, std::string shortName)
  : mPackage(package), mShortName(shortName) { }
  
  ~ModelTypeSpecifier() { }

  std::string getShortName() const override;
  
  std::string getName(IRGenerationContext& context) const override;
  
  Model* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};

} /* namespace wisey */

#endif /* ModelTypeSpecifier_h */
