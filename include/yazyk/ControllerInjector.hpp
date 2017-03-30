//
//  ControllerInjector.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerInjector_h
#define ControllerInjector_h

#include "yazyk/ControllerTypeSpecifier.hpp"
#include "yazyk/IExpression.hpp"

namespace yazyk {
  
/**
 * Represents injector used to initialize instantiate controllers
 */
class ControllerInjector : public IExpression {
    
  ControllerTypeSpecifier& mControllerTypeSpecifier;
  
public:
  
  ControllerInjector(ControllerTypeSpecifier& controllerTypeSpecifier) :
  mControllerTypeSpecifier(controllerTypeSpecifier) { }
  
  ~ControllerInjector() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;

private:
  
  llvm::Instruction* createMalloc(IRGenerationContext& context) const;

  std::string getVariableName() const;

  void initializeVTable(IRGenerationContext& context,
                        Controller* controller,
                        llvm::Instruction* malloc) const;
};
  
} /* namespace yazyk */

#endif /* ControllerInjector_h */
