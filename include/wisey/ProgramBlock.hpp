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

#include "wisey/IObjectDefinitionStatement.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents a top level block of statements
 */
class ProgramBlock {
  StatementList mStatements;
  ObjectDefinitionStatementList mObjectDefinitions;
  
public:
  ProgramBlock() { }
  
  ~ProgramBlock() { }
  
  StatementList& getStatements();
  
  ObjectDefinitionStatementList& getObjectDefinitions();
  
  void prototypeObjects(IRGenerationContext& context) const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const;
};

} /* namespace wisey */

#endif /* ProgramBlock_h */
