//
//  StoreInReference.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef StoreInReference_h
#define StoreInReference_h

#include "IExpression.hpp"
#include "IStatement.hpp"

namespace wisey {
  
  /**
   * Represents ::llvm::store() statement that stores value in a location pointed to by a pointer
   */
  class StoreInReference : public IStatement {
    
    IExpression* mValueExpression;
    IExpression* mPointerExpression;
    int mLine;
    
  public:
    
    StoreInReference(IExpression* valueExpression, IExpression* pointerExpression, int line);
    
    ~StoreInReference();
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* StoreInReference_h */
