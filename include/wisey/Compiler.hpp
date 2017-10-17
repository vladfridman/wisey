//
//  Compiler.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Compiler_h
#define Compiler_h

#include <llvm/ExecutionEngine/GenericValue.h>

#include "wisey/CompilerArguments.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramFile.hpp"

namespace wisey {
  
class IRGenerationContext;
  
/**
 * Represents Wisey language compiler
 */
class Compiler {
  IRGenerationContext mContext;
  CompilerArguments& mArguments;
  bool mHasCompiled;

public:
  
  Compiler(CompilerArguments& argments) : mArguments(argments), mHasCompiled(false) { }
  
  ~Compiler();
  
  /**
   * Compile Wisey code passed through constructor arguments
   */
  void compile();
  
  /**
   * Run compiled code
   */
  llvm::GenericValue run();
  
private:
  
  void printAssembly();
  
  void saveBinary(std::string outputFile);
  
  void extractHeaders(std::string headerFile);
  
  std::vector<ProgramFile*> parseFiles(std::vector<std::string> sourceFiles);
  
  void prototypeObjects(std::vector<ProgramFile*> programFiles, IRGenerationContext& context);
  
  void prototypeMethods(std::vector<ProgramFile*> programFiles, IRGenerationContext& context);
  
  void generateIR(std::vector<ProgramFile*> programFiles, IRGenerationContext& context);
  
  void deleteProgramFiles(std::vector<ProgramFile*> programFiles);
  
  llvm::Value* defineSourceFileConstant(std::string sourceFile);
  
};
  
} /* namespace wisey */

#endif /* Compiler_h */
