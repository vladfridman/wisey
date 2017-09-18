//
//  ProgramPrefix.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramPrefix_h
#define ProgramPrefix_h

#include <llvm/IR/Instructions.h>

#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Things that need to be run before the parsed program code
 */
class ProgramPrefix : public IStatement {

public:
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

private:
  
  void defineFreeIfNotNullFunction(IRGenerationContext& context) const;

  void defineNPEModel(IRGenerationContext& context) const;

  void defineNPEFunction(IRGenerationContext& context) const;
  
  void defineIProgramInterface(IRGenerationContext& context) const;
  
  llvm::StructType* defineFileStruct(IRGenerationContext& context) const;

  void defineStderr(IRGenerationContext& context, llvm::StructType* fileStructType) const;
  
};
  
} /* namespace wisey */

#endif /* ProgramPrefix_h */
