//
//  MethodSignature.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignature_h
#define MethodSignature_h

#include "yazyk/AccessLevel.hpp"
#include "yazyk/IMethodDescriptor.hpp"

namespace yazyk {
  
class MethodSignature : public IMethodDescriptor {
  
  std::string mName;
  AccessLevel mAccessLevel;
  IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<IType*> mThrownExceptions;
  unsigned long mIndex;

public:
  
  MethodSignature(std::string name,
                  AccessLevel accessLevel,
                  IType* returnType,
                  std::vector<MethodArgument*> arguments,
                  std::vector<IType*> thrownExceptions,
                  unsigned long index) :
  mName(name),
  mAccessLevel(accessLevel),
  mReturnType(returnType),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions),
  mIndex(index) { }
  
  ~MethodSignature() { mArguments.clear(); }
  
  /**
   * Creates a copy of the object with a different index
   */
  MethodSignature* createCopyWithIndex(unsigned long index) const;
  
  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  unsigned long getIndex() const override;
  
  std::vector<IType*> getThrownExceptions() const override;
  
};
  
} /* namespace yazyk */

#endif /* MethodSignature_h */
