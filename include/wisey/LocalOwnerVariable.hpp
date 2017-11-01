//
//  LocalOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalOwnerVariable_h
#define LocalOwnerVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectOwnerType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
/**
 * Represents a variable that is an object owner
 */
class LocalOwnerVariable : public IVariable {
    
  std::string mName;
  const IObjectOwnerType* mType;
  llvm::Value* mValue;
  bool mIsInitialized;
  
public:
  
  LocalOwnerVariable(std::string name, const IObjectOwnerType* type, llvm::Value* value);
  
  ~LocalOwnerVariable();
  
  std::string getName() const override;
  
  const IObjectOwnerType* getType() const override;
  
  llvm::Value* getValue() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context,
                                    std::string llvmVariableName) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;
  
  void free(IRGenerationContext& context) const override;
  
  bool existsInOuterScope() const override;
  
  void setToNull(IRGenerationContext& context) override;
  
};
  
} /* namespace wisey */

#endif /* LocalOwnerVariable_h */
