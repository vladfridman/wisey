//
//  BindActionGlobalStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef BindActionGlobalStatement_h
#define BindActionGlobalStatement_h

#include <string>

#include "wisey/BindAction.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/InjectionArgument.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a bind action binding aan interface to a controller globally for the whole program
   */
  class BindActionGlobalStatement : public IGlobalStatement {
    BindAction* mBindAction;
    
  public:
    
    BindActionGlobalStatement(BindAction* bindAction);
    
    ~BindActionGlobalStatement();
    
    IObjectType* prototypeObject(IRGenerationContext& context,
                                 ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* BindActionGlobalStatement_h */
