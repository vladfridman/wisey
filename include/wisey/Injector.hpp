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
#include "wisey/IObjectTypeSpecifier.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  /**
   * Represents injector used to initialize and instantiate controllers
   */
  class Injector : public IExpression {
    
    IObjectTypeSpecifier* mObjectTypeSpecifier;
    InjectionArgumentList mInjectionArgumentList;
    int mLine;
    
  public:
    
    Injector(IObjectTypeSpecifier* objectTypeSpecifier,
             InjectionArgumentList injectionArgumentList,
             int line);
    
    ~Injector();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* Injector_h */

