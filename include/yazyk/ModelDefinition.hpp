//
//  ModelDefinition.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelDefinition_h
#define ModelDefinition_h

#include "yazyk/Block.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/ModelFieldDeclaration.hpp"

namespace yazyk {
  
/**
 * Represents MODEL definition which is analogous to an immutable class in C++
 */
class ModelDefinition : public IStatement {
  const std::string mName;
  std::vector<ModelFieldDeclaration *>& mFields;
  
public:
  
  ModelDefinition(std::string name, std::vector<ModelFieldDeclaration*>& fields)
    : mName("model." + name), mFields(fields) { }
  
  ~ModelDefinition() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* ModelDefinition_h */