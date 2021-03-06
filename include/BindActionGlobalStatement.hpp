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

#include "BindAction.hpp"
#include "IGlobalStatement.hpp"
#include "InjectionArgument.hpp"
#include "InterfaceTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a bind action binding aan interface to a controller globally for the whole program
   */
  class BindActionGlobalStatement : public IGlobalStatement {
    BindAction* mBindAction;
    int mLine;
    
  public:
    
    BindActionGlobalStatement(BindAction* bindAction, int line);
    
    ~BindActionGlobalStatement();
    
    IObjectType* prototypeObject(IRGenerationContext& context,
                                 ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* BindActionGlobalStatement_h */
