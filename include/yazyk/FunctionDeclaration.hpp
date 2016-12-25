//
//  FunctionDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef FunctionDeclaration_h
#define FunctionDeclaration_h

#include "yazyk/Block.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/TypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

namespace yazyk {

/**
 * Represents a function declaration.
 *
 * A function contains Block that contains statements.
 */
class FunctionDeclaration : public IStatement {
  const TypeSpecifier& mType;
  const Identifier& mId;
  VariableList mArguments;
  Block& mBlock;
  
public:
  FunctionDeclaration(const TypeSpecifier& type,
                      const Identifier& id,
                      const VariableList& arguments,
                      Block& block) :
  mType(type), mId(id), mArguments(arguments), mBlock(block) { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};

} /* namespace yazyk */

#endif /* FunctionDeclaration_h */
