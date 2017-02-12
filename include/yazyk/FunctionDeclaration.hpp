//
//  FunctionDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FunctionDeclaration_h
#define FunctionDeclaration_h

#include "yazyk/IStatement.hpp"
#include "yazyk/MethodDeclaration.hpp"

namespace yazyk {

/**
 * Represents a global function declaration
 *
 * TODO: remove this class when global functions are removed
 */
class FunctionDeclaration : public IStatement {
  MethodDeclaration* mMethodDeclaration;
  
public:
  
  FunctionDeclaration(MethodDeclaration* methodDeclaration)
    : mMethodDeclaration(methodDeclaration) { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
};
  
} /* namespace yazyk */

#endif /* FunctionDeclaration_h */
