//
//  ProgramBlock.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramBlock_h
#define ProgramBlock_h

#include "wisey/Block.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {
  
/**
 * Represents one Wisey progarm file
 */
class ProgramFile : public IStatement {

  std::string mPackage;
  Block* mBlock;
  
public:
  
  ProgramFile(std::string package, Block* block) : mPackage(package), mBlock(block) { }
  
  ~ProgramFile();
  
  void prototype(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* ProgramBlock_h */
