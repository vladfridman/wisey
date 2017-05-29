//
//  ProgramBlock.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramFile_h
#define ProgramFile_h

#include "wisey/IStatement.hpp"
#include "wisey/ProgramBlock.hpp"

namespace wisey {

/**
 * Represents one Wisey progarm file
 */
class ProgramFile : public IStatement {

  std::string mPackage;
  ProgramBlock* mProgramBlock;
  
public:
  
  ProgramFile(std::string package, ProgramBlock* programBlock)
  : mPackage(package), mProgramBlock(programBlock) { }
  
  ~ProgramFile();
  
  /**
   * Generate object prototypes from controller, model and interface definitions
   */  
  void prototypeObjects(IRGenerationContext& context) const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* ProgramFile_h */
