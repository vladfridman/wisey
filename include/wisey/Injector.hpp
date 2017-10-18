//
//  Injector.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Injector_h
#define Injector_h

#include "wisey/IExpression.hpp"
#include "wisey/IInjectableObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents injector used to initialize and instantiate controllers
 */
class Injector : public IExpression {
    
  IInjectableObjectTypeSpecifier* mInjectableObjectTypeSpecifier;
  
public:
  
  Injector(IInjectableObjectTypeSpecifier* injectableObjectTypeSpecifier);
  
  ~Injector();
  
  IVariable* getVariable(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IObjectOwnerType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  void addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* Injector_h */
