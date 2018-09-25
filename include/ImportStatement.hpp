//
//  ImportStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ImportStatement_h
#define ImportStatement_h

#include <llvm/IR/Instructions.h>

#include "IGlobalStatement.hpp"
#include "IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Reprensets an import statement
   */
  class ImportStatement : public IGlobalStatement {
    
    IObjectTypeSpecifier* mTypeSpecifier;
    
  public:
    
    ImportStatement(IObjectTypeSpecifier* typeSpecifier);
    
    ~ImportStatement();
    
    IObjectType* prototypeObject(IRGenerationContext& context,
                                 ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
}


#endif /* ImportInterfaceStatement_h */

