//
//  Identifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Identifier_h
#define Identifier_h

#include "wisey/IExpression.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Represents an identifier in a wisey program.
 *
 * Loads value of the variable named 'name' into an LLVM variable named llvmVariableName
 */
class Identifier : public IExpression {
  
  const std::string mName;
  
public:
  Identifier(const std::string& name);
  
  ~Identifier();
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  const std::string& getName() const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* Identifier_h */
