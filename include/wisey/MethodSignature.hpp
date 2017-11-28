//
//  MethodSignature.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignature_h
#define MethodSignature_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectElement.hpp"

namespace wisey {
  
class MethodSignature : public IMethodDescriptor, public IObjectElement {
  
  std::string mName;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const Model*> mThrownExceptions;

public:
  
  MethodSignature(std::string name,
                  const IType* returnType,
                  std::vector<MethodArgument*> arguments,
                  std::vector<const Model*> thrownExceptions);
  
  ~MethodSignature();
  
  /**
   * Creates a copy of the object with a different index
   */
  MethodSignature* createCopy() const;

  bool isStatic() const override;
  
  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  std::vector<const Model*> getThrownExceptions() const override;
  
  ObjectElementType getObjectElementType() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* MethodSignature_h */
