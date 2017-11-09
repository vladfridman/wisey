//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Things that need to be run after the parsed program code
 */
class ProgramSuffix : public IStatement {
    
public:
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

private:

  void composeNPEFunctionBody(IRGenerationContext& context) const;
  
  void composeAddjustReferenceCounterForConcreteObjectUnsafelyFunctionBody(IRGenerationContext&
                                                                           context) const;
  
  void composeAdjustReferenceCounterForInterfaceFunctionBody(IRGenerationContext&
                                                                      context) const;
  
  llvm::Value* generateMain(IRGenerationContext& context,
                            InterfaceTypeSpecifier* programInterfaceSpecifier) const;
};
  
} /* namespace wisey */

