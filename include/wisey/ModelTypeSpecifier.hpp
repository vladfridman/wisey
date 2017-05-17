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

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {

/**
 * Represents MODEL type specifier
 */
class ModelTypeSpecifier : public ITypeSpecifier {
  const std::string mName;

public:
  
  ModelTypeSpecifier(std::string name) : mName(name) { }
  
  ~ModelTypeSpecifier() { }
  
  const std::string getName() const;

  IType* getType(IRGenerationContext& context) const override;
};

} /* namespace wisey */

#endif /* ModelTypeSpecifier_h */
