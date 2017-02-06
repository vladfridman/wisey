//
//  ModelTypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelTypeSpecifier_h
#define ModelTypeSpecifier_h

#include <string>

#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {

/**
 * Represents MODEL type specifier
 */
class ModelTypeSpecifier : public ITypeSpecifier {
  const std::string mName;

public:
  
  ModelTypeSpecifier(std::string name) : mName(name) { }
  
  ~ModelTypeSpecifier() { }
  
  const std::string getName() const;

  llvm::Type* getLLVMType(IRGenerationContext& context) const override;

  VariableStorageType getStorageType() const override;
};

} /* namespace yazyk */

#endif /* ModelTypeSpecifier_h */
