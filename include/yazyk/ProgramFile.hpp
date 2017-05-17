//
//  ProgramBlock.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramBlock_h
#define ProgramBlock_h

#include "yazyk/Block.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {
  
/**
 * Represents one Wisey progarm file
 */
class ProgramFile : public IStatement {

  std::string mPackage;
  Block* mBlock;
  
public:
  
  ProgramFile(std::string package, Block* block) : mPackage(package), mBlock(block) { }
  
  ~ProgramFile();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace yazyk */

#endif /* ProgramBlock_h */
