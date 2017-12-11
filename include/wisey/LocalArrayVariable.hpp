//
//  LocalArrayVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalArrayVariable_h
#define LocalArrayVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents a local variable of array type
 */
class LocalArrayVariable : public IVariable {
    
  std::string mName;
  const ArrayType* mType;
  llvm::Value* mValueStore;
  
public:
  
  LocalArrayVariable(std::string name, const ArrayType* type, llvm::Value* valueStore);
  
  ~LocalArrayVariable();
  
  std::string getName() const override;
  
  const ArrayType* getType() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression,
                                    int line) override;
  
};

} /* namespace wisey */

#endif /* LocalArrayVariable_h */
