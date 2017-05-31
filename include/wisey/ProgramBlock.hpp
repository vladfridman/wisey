//
//  ProgramBlock.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramBlock_h
#define ProgramBlock_h

#include <llvm/IR/Instructions.h>

#include "wisey/IGlobalStatement.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents a top level block of statements
 */
class ProgramBlock {
  StatementList mStatements;
  GlobalStatementList mGlobalStatementList;
  
public:
  ProgramBlock() { }
  
  ~ProgramBlock() { }
  
  StatementList& getStatements();
  
  GlobalStatementList& getGlobalStatements();
  
  /**
   * Generate object prototypes from controller, model and interface definitions
   */
  void prototypeObjects(IRGenerationContext& context) const;
  
  /**
   * Generate method information for prototyped objects
   */
  void prototypeMethods(IRGenerationContext& context) const;
  
  /**
   * Generate LLVM intermediate representation code for this program block
   */
  llvm::Value* generateIR(IRGenerationContext& context) const;
};

} /* namespace wisey */

#endif /* ProgramBlock_h */
