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
 * Represents an identifier in a yazyk program.
 *
 * Loads value of the variable named 'name' into an LLVM variable named llvmVariableName
 */
class Identifier : public IExpression {
  
  const std::string mName;
  std::string mLLVMVariableName;
  
public:
  Identifier(const std::string& name) : mName(name), mLLVMVariableName("") { }
  
  Identifier(const std::string& name, const std::string& llvmVariableName) :
    mName(name), mLLVMVariableName(llvmVariableName) { }
  
  ~Identifier() { }
  
  const std::string& getName() const;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
private:
  
  IVariable* checkGetVariable(IRGenerationContext& context) const;
};
  
} /* namespace wisey */

#endif /* Identifier_h */
