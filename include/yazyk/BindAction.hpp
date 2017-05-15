//
//  BindAction.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BindAction_h
#define BindAction_h

#include <string>

#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents a bind action binding a controller to an interface
 */
class BindAction : public IStatement {
  std::string mControllerId;
  std::string mInterfaceId;
  
public:
  
  BindAction(std::string controllerId, std::string interfaceId)
  : mControllerId(controllerId), mInterfaceId(interfaceId) { }
  
  ~BindAction() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace yazyk */

#endif /* BindAction_h */
