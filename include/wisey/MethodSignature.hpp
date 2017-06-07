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

namespace wisey {
  
class MethodSignature : public IMethodDescriptor {
  
  std::string mName;
  AccessLevel mAccessLevel;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const IType*> mThrownExceptions;
  unsigned long mIndex;

public:
  
  MethodSignature(std::string name,
                  AccessLevel accessLevel,
                  const IType* returnType,
                  std::vector<MethodArgument*> arguments,
                  std::vector<const IType*> thrownExceptions,
                  unsigned long index) :
  mName(name),
  mAccessLevel(accessLevel),
  mReturnType(returnType),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions),
  mIndex(index) { }
  
  ~MethodSignature();
  
  /**
   * Creates a copy of the object with a different index
   */
  MethodSignature* createCopyWithIndex(unsigned long index) const;
  
  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  unsigned long getIndex() const override;
  
  std::vector<const IType*> getThrownExceptions() const override;
  
};
  
} /* namespace wisey */

#endif /* MethodSignature_h */
