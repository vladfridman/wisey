//
//  ModelFieldDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelFieldDeclaration_h
#define ModelFieldDeclaration_h

#include <llvm/IR/Type.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {

/**
 * Represents a field in MODEL definition
 */
class ModelFieldDeclaration {

  ITypeSpecifier* mTypeSpecifier;
  const std::string mName;

public:
  
  ModelFieldDeclaration(ITypeSpecifier* typeSpecifier, const std::string& name)
    : mTypeSpecifier(typeSpecifier), mName(name) { }
  
  ~ModelFieldDeclaration();
  
  ITypeSpecifier* getTypeSpecifier();
  
  std::string getName();
};
  
} /* namespace wisey */

#endif /* ModelFieldDeclaration_h */
