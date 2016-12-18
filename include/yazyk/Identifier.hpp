//
//  Identifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Identifier_h
#define Identifier_h

#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Represents an identifier in a yazyk program.
 *
 * Generated IR loads instance of the variable containing value of the identifier
 */
class Identifier : public IExpression {
  
  const std::string mName;
  std::string mVariableName;
  
public:
  Identifier(const std::string& name) : mName(name), mVariableName("") { }
  
  Identifier(const std::string& name, const std::string& variableName) :
    mName(name), mVariableName(variableName) { }
  
  ~Identifier() { }
  
  const std::string& getName() const;
  
  llvm::Value* generateIR(IRGenerationContext& context) override;
};
  
} /* namespace yazyk */

#endif /* Identifier_h */
