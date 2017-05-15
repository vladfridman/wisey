//
//  InterfaceInjector.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceInjector_h
#define InterfaceInjector_h

#include "yazyk/ControllerTypeSpecifier.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/InterfaceTypeSpecifier.hpp"

namespace yazyk {
  
/**
 * Represents injector used to initialize and instantiate controllers bound to interfaces
 */
class InterfaceInjector : public IExpression {
    
  InterfaceTypeSpecifier& mInterfaceTypeSpecifier;
  
public:
  
  InterfaceInjector(InterfaceTypeSpecifier& interfaceTypeSpecifier) :
  mInterfaceTypeSpecifier(interfaceTypeSpecifier) { }
  
  ~InterfaceInjector() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
private:
  
  std::string getVariableName() const;
};
  
} /* namespace yazyk */

#endif /* InterfaceInjector_h */
