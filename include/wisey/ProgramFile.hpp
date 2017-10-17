//
//  ProgramFile.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ProgramFile_h
#define ProgramFile_h

#include "wisey/IGlobalStatement.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {

/**
 * Represents one Wisey progarm file
 */
class ProgramFile : public IStatement {

  std::string mPackage;
  GlobalStatementList mGlobalStatementList;
  std::string mSourceFile;
  
public:
  
  ProgramFile(std::string package, GlobalStatementList globalStatementList);
  
  ~ProgramFile();
  
  /**
   * Generate object prototypes from controller, model and interface definitions
   */  
  void prototypeObjects(IRGenerationContext& context) const;
  
  /**
   * Generate method information for prototyped objects
   */
  void prototypeMethods(IRGenerationContext& context) const;
  
  /**
   * Sets source file name pointer
   */
  void setSourceFile(std::string sourceFile);

  /**
   * Returns source file name pointer
   */
  std::string getSourceFile() const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  static std::string getSourceFileConstantName(std::string sourceFile);
  
};

} /* namespace wisey */

#endif /* ProgramFile_h */
