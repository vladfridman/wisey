//
//  Catch.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Catch_h
#define Catch_h

#include "wisey/CompoundStatement.hpp"
#include "wisey/IModelTypeSpecifier.hpp"

namespace wisey {

/**
 * Represents a single catch in a try/catch statement
 */
class Catch {
  IModelTypeSpecifier* mModelTypeSpecifier;
  std::string mIdentifier;
  CompoundStatement* mCompoundStatement;
  int mLine;
  
public:
  
  Catch(IModelTypeSpecifier* modelTypeSpecifier,
        std::string identifier,
        CompoundStatement* compoundStatement,
        int line);
  
  ~Catch();
  
  /**
   * Returns the object type corresponding to the exception in this catch clause
   */
  ModelOwner* getType(IRGenerationContext& context) const;
  
  /**
   * Generate IR for this catch for a given exception and add it to the given basic block.
   * Returns true if the catch ends with a terminator
   */
  bool generateIR(IRGenerationContext& context,
                  llvm::Value* wrappedException,
                  llvm::BasicBlock* catchBlock,
                  llvm::BasicBlock* exceptionContinueBlock) const;
};
  
} /* namespace wisey */

#endif /* Catch_h */
