//
//  EssentialFunctions.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef EssentialFunctions_h
#define EssentialFunctions_h

#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
  /**
   * Registers essential functions that are neccessary for code generation
   */
  class EssentialFunctions : public IStatement {
    
  public:
    
    ~EssentialFunctions();
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* EssentialFunctions_h */
