//
//  InterfaceDefinition.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceDefinition_h
#define InterfaceDefinition_h

#include "yazyk/Block.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/MethodSignature.hpp"
#include "yazyk/ModelFieldDeclaration.hpp"

namespace yazyk {
  
/**
 * Represents an INTERFACE definition which is analogous to a pure virtual class in C++
 */
class InterfaceDefinition : public IStatement {
  const std::string mName;
  std::vector<MethodSignature *>& mMethods;
  
public:
  
  InterfaceDefinition(std::string name,
                      std::vector<MethodSignature *>& methods)
  : mName(name), mMethods(methods) { }
  
  ~InterfaceDefinition() { };
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* InterfaceDefinition_h */
