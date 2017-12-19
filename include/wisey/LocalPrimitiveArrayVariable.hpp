//
//  LocalPrimitiveArrayVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalPrimitiveArrayVariable_h
#define LocalPrimitiveArrayVariable_h

#include "wisey/ArrayType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents a local variable of array type where each element is of a primitive type
 */
class LocalPrimitiveArrayVariable : public IVariable {
    
  std::string mName;
  const ArrayType* mType;
  llvm::Value* mValueStore;
  
public:
  
  LocalPrimitiveArrayVariable(std::string name, const ArrayType* type, llvm::Value* valueStore);
  
  ~LocalPrimitiveArrayVariable();
  
  std::string getName() const override;
  
  const ArrayType* getType() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression,
                                    std::vector<const IExpression*> arrayIndices,
                                    int line) override;
  
};

} /* namespace wisey */

#endif /* LocalPrimitiveArrayVariable_h */
