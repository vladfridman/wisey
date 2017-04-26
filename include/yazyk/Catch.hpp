//
//  Catch.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Catch_h
#define Catch_h

#include "yazyk/ITypeSpecifier.hpp"
#include "yazyk/IStatement.hpp"

namespace yazyk {

/**
 * Represents a single catch in a try/catch statement
 */
class Catch {
  ITypeSpecifier& mTypeSpecifier;
  std::string mIdentifier;
  IStatement& mStatement;
  
public:
  
  Catch(ITypeSpecifier& typeSpecifier, std::string identifier, IStatement& statement)
  : mTypeSpecifier(typeSpecifier), mIdentifier(identifier), mStatement(statement) { }
  
  ~Catch() { }
  
  /**
   * Returns the object type corresponding to the exception in this catch clause
   */
  Model* getType(IRGenerationContext& context) const;
  
  /**
   * Generate IR for this catch for a given exception and add it to the given basic block
   */
  void generateIR(IRGenerationContext& context,
                  llvm::Value* wrappedException,
                  llvm::BasicBlock* catchBlock) const;
};
  
} /* namespace yazyk */

#endif /* Catch_h */
