//
//  LocalReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalReferenceVariable_h
#define LocalReferenceVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Represents a variable that is a reference to an owner object
 */
class LocalReferenceVariable : public IVariable {
  
  std::string mName;
  const IObjectType* mType;
  llvm::Value* mValueStore;
  bool mIsInitialized;
  
public:
  
  LocalReferenceVariable(std::string name, const IObjectType* type, llvm::Value* valueStore);
  
  ~LocalReferenceVariable();
  
  std::string getName() const override;
  
  const IObjectType* getType() const override;
  
  llvm::Value* generateIdentifierIR(IRGenerationContext& context) const override;
  
  llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                    IExpression* assignToExpression) override;

  void free(IRGenerationContext& context) const override;
  
  bool existsInOuterScope() const override;

};

} /* namespace wisey */

#endif /* LocalReferenceVariable_h */
