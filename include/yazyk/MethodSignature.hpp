//
//  MethodSignature.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignature_h
#define MethodSignature_h

#include "yazyk/AccessSpecifier.hpp"
#include "yazyk/IMethodDescriptor.hpp"

namespace yazyk {
  
class MethodSignature : public IMethodDescriptor {
  
  std::string mName;
  AccessSpecifier mAccessSpecifier;
  IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  unsigned long mIndex;
  
public:
  
  MethodSignature(std::string name,
                  AccessSpecifier accessSpecifier,
                  IType* returnType,
                  std::vector<MethodArgument*> arguments,
                  unsigned long index) :
  mName(name),
  mAccessSpecifier(accessSpecifier),
  mReturnType(returnType),
  mArguments(arguments),
  mIndex(index) { }
  
  ~MethodSignature() { mArguments.clear(); }
  
  /**
   * Creates a copy of the object with a different index
   */
  MethodSignature* createCopyWithIndex(unsigned long index) const;
  
  std::string getName() const override;
  
  AccessSpecifier getAccessSpecifier() const override;
  
  IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  unsigned long getIndex() const override;
  
};
  
} /* namespace yazyk */

#endif /* MethodSignature_h */
