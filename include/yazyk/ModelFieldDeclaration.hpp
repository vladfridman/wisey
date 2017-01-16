//
//  ModelFieldDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelFieldDeclaration_h
#define ModelFieldDeclaration_h

#include <llvm/IR/Type.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {

/**
 * Represents a field in MODEL definition
 */
class ModelFieldDeclaration {

  ITypeSpecifier& mType;
  const std::string mName;

public:
  
  ModelFieldDeclaration(ITypeSpecifier& type, const std::string& name)
    : mType(type), mName(name) { }
  
  ~ModelFieldDeclaration() { }
  
  ITypeSpecifier& getType();
};
  
} /* namespace yazyk */

#endif /* ModelFieldDeclaration_h */
