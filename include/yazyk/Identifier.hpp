//
//  Identifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Identifier_h
#define Identifier_h

#include "yazyk/IExpression.hpp"
#include "yazyk/IHasType.hpp"

namespace yazyk {

/**
 * Represents an identifier in a yazyk program.
 *
 * Loads value of the variable named 'name' into an LLVM variable named llvmVariableName
 */
class Identifier : public IExpression, IHasType {
  
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
  
private:
  
  Variable* checkGetVariable(IRGenerationContext& context) const;
};
  
} /* namespace yazyk */

#endif /* Identifier_h */
