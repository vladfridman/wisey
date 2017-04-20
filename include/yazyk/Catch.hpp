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
};
  
} /* namespace yazyk */

#endif /* Catch_h */
