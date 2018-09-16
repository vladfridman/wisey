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
    
    Compiler(CompilerArguments& argments);
    
    ~Compiler();
    
    /**
     * Maximum number of errors that compiler may encounter before it terminates
     */
    static const unsigned int MAX_NUMBER_OF_ERRORS;

    /**
     * Compile Wisey code passed through constructor arguments
     */
    void compile();

    /**
     * Optimize generated IR code
     */
    void optimize();
    
    /**
     * Prints llvm code
     */
    void printAssembly();
    
    /**
     * Saves resulting IR in an object file
     */
    void saveBinary(std::string outputFile);
    
    /**
     * Checks that wisey.lang.IProgram
     */

    /**
     * Run compiled code
     */
    int run(int argc, char** argv);

  private:
    
    void extractHeaders(std::string headerFile);
    
    std::vector<ProgramFile*> parseFiles(std::vector<std::string> sourcePatterns);
    
    void prototypeObjects(std::vector<ProgramFile*> programFiles, IRGenerationContext& context);
    
    void prototypeMethods(std::vector<ProgramFile*> programFiles, IRGenerationContext& context);
    
    void generateIR(std::vector<ProgramFile*> programFiles, IRGenerationContext& context);
    
    void deleteProgramFiles(std::vector<ProgramFile*> programFiles);
    
  private:
    
    std::vector<std::string> expandPatterns(std::vector<std::string> sourcePatterns);
    
    std::vector<std::string> listFilesInDirectory(std::string directory);
    
    void appendFilesFromDirectoryThatMatch(std::string directory,
                                           std::string pattern,
                                           std::vector<std::string>& sourceFiles);
    
    bool wildcardMatch(char const *needle, char const *haystack);
    
  };
  
} /* namespace wisey */

#endif /* Compiler_h */

