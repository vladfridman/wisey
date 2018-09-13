#!/usr/bin/python

import sys
import sys, getopt
import collections
import operator
import re

def docObjectName(file, objectName, dictionary):
  if objectName.endswith('*'):
    realObjectName = objectName[:-1]
    ending = '*'
  else:
    realObjectName = objectName
    ending = ''
  if realObjectName in dictionary:
    fileName = dictionary[realObjectName].getFileName()
    file.write('<a href="' + fileName + '">' + realObjectName + '</a>' + ending)
  else:
    file.write(objectName)

class Object:
  mPackage = ""
  mType = ""
  mName = ""
  mDescription = ""
  mReceivedFields = [];
  mMethods = []
  mStaticMethods = []
  mSubObjects = []
  mImplements = []
  mConstants = []
  mParentObject = None

  def __init__(self, package, objectType, objectName, description):
    self.mPackage = package
    self.mType = objectType
    self.mName = objectName
    self.mDescription = description
    self.mReceivedFields = []
    self.mMethods = []
    self.mStaticMethods = []
    self.mSubObjects = []
    self.mImplements = []
    self.mConstants = []
    self.mParentObject = None

  def addReceivedField(self, field):
    self.mReceivedFields.append(field)

  def addMethod(self, method):
    self.mMethods.append(method)

  def addConstant(self, constant):
    self.mConstants.append(constant)

  def addImplements(self, implements):
    self.mImplements.extend(implements)

  def addStaticMethod(self, method):
    self.mStaticMethods.append(method)

  def addSubObject(self, subObject):
    self.mSubObjects.append(subObject)
    subObject.mParentObject = self

  def addSubObjectReceivedField(self, field):
    self.mSubObjects[0].addReceivedField(field)

  def addSubObjectMethod(self, method):
    self.mSubObjects[0].addMethod(method)

  def addSubObjectStaticMethod(self, method):
    self.mSubObjects[0].addStaticMethod(method)

  def getFullName(self):
    if self.mParentObject == None:
      return self.mPackage + "." + self.mName
    else:
      return self.mParentObject.getFullName() + "." + self.mName

  def getFileName(self):
    return self.getFullName() + ".html"

  def printShortDoc(self, file):
    file.write('        <tr>\n')
    file.write('          <td class="elementDefinitionLeft">\n')
    file.write('            ' + self.mType + '\n')
    file.write('          </td>\n')
    file.write('          <td class="elementDefinitionRight">\n')
    file.write('            <a href="' + self.getFileName() + '">' + self.getFullName() + '</a>')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementSeparator" colspan="2">&nbsp;</td>\n')
    file.write('        </tr>\n')
    for subObject in self.mSubObjects:
      subObject.printShortDoc(file)


  def printDoc(self, destinationDir, dictionary):
    for subObject in self.mSubObjects:
      subObject.printDoc(destinationDir, dictionary)

    from os.path import isfile, join
    fileName = join(destinationDir, self.getFileName())
    file = open(fileName, "w")
    print fileName
    file.write('<html>\n')
    file.write('<head>\n')
    file.write('<link href="wiseydoc.css" rel="stylesheet" type="text/css">\n')
    file.write('</head>\n')
    file.write('<body>\n')
    file.write('  <div class="contents">\n')
    file.write('    <a href="index.html">Index</a>')
    file.write('  </div>\n')
    file.write('  <div class="contents">\n')
    file.write('    <table class="docSection">\n')
    file.write('      <tbody>\n')
    file.write('        <tr class="header">\n')
    file.write('          <td colspan="2">\n')
    file.write('            <h2 class="sectionHeader">Object</h2>\n')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementDefinitionLeft">\n')
    file.write('            ' + self.mType + '\n')
    file.write('          </td>\n')
    file.write('          <td class="elementDefinitionRight">\n')
    file.write('            ' + self.getFullName() + '\n')
    if len(self.mImplements) > 0:
      file.write(' implements ')
      for interface in self.mImplements:
        docObjectName(file, interface, dictionary)
        if interface != self.mImplements[-1]:
          file.write(', ')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementDescriptionLeft">\n')
    file.write('            \n')
    file.write('          </td>\n')
    file.write('          <td class="elementDescriptionRight">\n')
    file.write('            ' + self.mDescription + '\n')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('      </tbody>\n')
    file.write('    </table>\n')

    if (len(self.mConstants)):
      file.write('    <table class="docSection">\n')
      file.write('      <tbody>\n')
      file.write('        <tr class="header">\n')
      file.write('          <td colspan="2">\n')
      file.write('            <h2 class="sectionHeader">Constants</h2>\n')
      file.write('          </td>\n')
      file.write('        </tr>\n')
      for constant in self.mConstants:
        constant.printDoc(file, dictionary)
      file.write('      </tbody>\n')
      file.write('    </table>\n')

    if (len(self.mReceivedFields)):
      file.write('    <table class="docSection">\n')
      file.write('      <tbody>\n')
      file.write('        <tr class="header">\n')
      file.write('          <td colspan="2">\n')
      file.write('            <h2 class="sectionHeader">Received Fields</h2>\n')
      file.write('          </td>\n')
      file.write('        </tr>\n')
      for field in self.mReceivedFields:
        field.printDoc(file, dictionary)
      file.write('      </tbody>\n')
      file.write('    </table>\n')

    if (len(self.mMethods)):
      file.write('    <table class="docSection">\n')
      file.write('      <tbody>\n')
      file.write('        <tr class="header">\n')
      file.write('          <td colspan="2">\n')
      file.write('            <h2 class="sectionHeader">Public Methods</h2>\n')
      file.write('          </td>\n')
      file.write('        </tr>\n')
      for method in self.mMethods:
        method.printDoc(file, dictionary)
      file.write('      </tbody>\n')
      file.write('    </table>\n')

    if (len(self.mStaticMethods)):
      file.write('    <table class="docSection">\n')
      file.write('      <tbody>\n')
      file.write('        <tr class="header">\n')
      file.write('          <td colspan="2">\n')
      file.write('            <h2 class="sectionHeader">Public Static Methods</h2>\n')
      file.write('          </td>\n')
      file.write('        </tr>\n')
      for method in self.mStaticMethods:
        method.printDoc(file, dictionary)
      file.write('      </tbody>\n')
      file.write('    </table>\n')

    if (len(self.mSubObjects)):
      file.write('    <table class="docSection">\n')
      file.write('      <tbody>\n')
      file.write('        <tr class="header">\n')
      file.write('          <td colspan="2">\n')
      file.write('            <h2 class="sectionHeader">Inner Objects</h2>\n')
      file.write('          </td>\n')
      file.write('        </tr>\n')
      for subObject in self.mSubObjects:
        subObject.printShortDoc(file)
      file.write('      </tbody>\n')
      file.write('    </table>\n')


    file.write('  </div>\n')
    file.write('</body>\n')
    file.write('</html>\n')
    file.close()


  def getName(self):
    return self.mName

class Method:
  mType = "";
  mName = "";
  mParameters = []
  mThrows = []
  mDescription = ""
  mIsOverride = False
  mOverridenMethod = None
  mOverrideFromObject = None
  
  def __init__(self, methodType, methodName, methodParams, description):
    self.mType = methodType
    self.mName = methodName
    self.mParameters = methodParams
    self.mDescription = description
    self.mThrows = []
    self.mIsOverride = False
    self.mOverridenMethod = None
    self.mOverrideFromObject = None

  def addThrows(self, throws):
    self.mThrows.extend(throws)

  def setOverride(self):
    self.mIsOverride = True

  def printDoc(self, file, dictionary):
    file.write('        <tr>\n')
    file.write('          <td class="elementDefinitionLeft">\n')
    file.write('            ')
    docObjectName(file, self.mType, dictionary)
    file.write('          </td>\n')
    file.write('          <td class="elementDefinitionRight">\n')
    file.write('            ' + self.mName + ' (')
    for parameter in self.mParameters:
      if parameter[0] == 0:
        parameterType = parameter[1]
      else:
        parameterType = parameter[0] + ' ' + parameter[1]
      if parameterType.strip() == 'throws':
        continue
      docObjectName(file, parameterType.strip(), dictionary)
      file.write(' ' + parameter[2])
      if (self.mParameters[-1] != parameter):
        file.write(',')
    file.write(') ')
    if len(self.mThrows) > 0:
      file.write('throws ')
      for throw in self.mThrows:
        docObjectName(file, throw, dictionary)
        if throw != self.mThrows[-1]:
          file.write(', ')
    if self.mIsOverride == True:
      if self.mOverrideFromObject != None:
        file.write(' override of ')
        file.write('<a href="' + self.mOverrideFromObject.getFileName() + '">' + self.mOverrideFromObject.mName + '</a>.' + self.mName + '()')
      else:
        file.write(' override ')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementDescriptionLeft">\n')
    file.write('            \n')
    file.write('          </td>\n')
    file.write('          <td class="elementDescriptionRight">\n')
    if len(self.mDescription):
      file.write('            ' + self.mDescription + '\n')
    elif self.mOverridenMethod != None:
      file.write('            ' + self.mOverridenMethod.mDescription + '\n')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementSeparator" colspan="2">&nbsp;</td>\n')
    file.write('        </tr>\n')

class Field:
  mName = ""
  mType = ""
  mDescription = ""

  def __init__(self, fieldType, fieldName, description):
    self.mType = fieldType
    self.mName = fieldName
    self.mDescription = description

  def printDoc(self, file, dictionary):
    file.write('        <tr>\n')
    file.write('          <td class="elementDefinitionLeft">\n')
    file.write('            ')
    docObjectName(file, self.mType, dictionary)
    file.write('          </td>\n')
    file.write('          <td class="elementDefinitionRight">\n')
    file.write('            ' + self.mName + '\n')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementDescriptionLeft">\n')
    file.write('            \n')
    file.write('          </td>\n')
    file.write('          <td class="elementDescriptionRight">\n')
    file.write('            ' + self.mDescription + '\n')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementSeparator" colspan="2">&nbsp;</td>\n')
    file.write('        </tr>\n')

class Constant:
  mName = ""
  mType = ""
  mValue = ""
  mDescription = ""


  def __init__(self, constantType, constntName, constantValue, description):
    self.mType = constantType
    self.mName = constntName
    self.mValue = constantValue
    self.mDescription = description

  def printDoc(self, file, dictionary):
    file.write('        <tr>\n')
    file.write('          <td class="elementDefinitionLeft">\n')
    file.write('            ')
    docObjectName(file, self.mType, dictionary)
    file.write('          </td>\n')
    file.write('          <td class="elementDefinitionRight">\n')
    file.write('            ' + self.mName + ' = ' + self.mValue + '\n')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementDescriptionLeft">\n')
    file.write('            \n')
    file.write('          </td>\n')
    file.write('          <td class="elementDescriptionRight">\n')
    file.write('            ' + self.mDescription + '\n')
    file.write('          </td>\n')
    file.write('        </tr>\n')
    file.write('        <tr>\n')
    file.write('          <td class="elementSeparator" colspan="2">&nbsp;</td>\n')
    file.write('        </tr>\n')


def readFile(path):
  packageMatch = re.compile("package\s([a-zA-Z0-9_.]+);")
  commentStart = re.compile("\/\*+\s*$")
  commentLine = re.compile("\*+\s(\s*.*)$")
  commentEnd = re.compile("\*+\/")
  objectStart = re.compile("(public\s+)?(controller|model|interface|node)\s+([a-zA-Z0-9_]+)\s")
  methodStart = re.compile("public\s+(immutable\s+)?([a-zA-Z0-9_:*\[\]]+)\s+([a-zA-Z0-9_]+)\(")
  staticMethodStart = re.compile("public\s+static\s+(immutable\s+)?([a-zA-Z0-9_:*\[\]]+)\s+([a-zA-Z0-9_]+)\(")
  interfaceMethodStart = re.compile("(immutable\s+)?([a-zA-Z0-9_:*\[\]]+)\s+([a-zA-Z0-9_]+)\(")
  parameter = re.compile("(immutable\s+)?([a-zA-Z0-9_:*\[\]]+)\s+([a-zA-Z0-9_]+)[\),]")
  methodEnd = re.compile("\)")
  fieldMatch = re.compile("(receive\s+)(immutable\s+)?([a-zA-Z0-9_:*\[\]]+)\s+([a-zA-Z0-9_]+);")
  typeList = re.compile("([a-zA-Z0-9_:*\[\]]+)\s*[,\{]")
  typeListEnd = re.compile("\{")
  constantMatch = re.compile("public constant ([a-zA-Z0-9_:*\[\]]+) ([A-Z0-9_]+) = (.*);")

  state = 0;
  inSubObject = 0;
  comment = '';
  package = '';
  objects = []
  methods = []
  parameters = []
  throws = []

  with open(path) as f_in:
    for line in f_in:
      if state == 0:
        result = re.search(packageMatch, line)
        if result != None:
          package = result.groups(0)[0]
          state = 1
      elif state == 1:
        result = re.search(commentStart, line)
        if result != None:
          state = 2
          comment = '';
      elif state == 2:
        result = re.search(commentLine, line)
        if result != None:
          comment += result.groups(0)[0] + '\n'
        result = re.search(commentEnd, line)
        if result != None:
          state = 1
      if state == 1:
        result = re.search(objectStart, line)
        if result != None:
          state = 1
          objectAcces = result.groups(0)[0]
          objectType = result.groups(0)[1]
          objectName = result.groups(0)[2]
          o = Object(package, objectType, objectName, comment)
          comment = '';
          if objectAcces != 0:
            objects[0].addSubObject(o)
            inSubObject = 1;
          else:
            objects.append(o)
            inSubObject = 0;
          methods = []
          if line.find(" implements ") > 0:
            implementsLine = line[line.find(" implements ") + 12:]
            result = re.findall(typeList, implementsLine)
            o.addImplements(result)
            if re.search(typeListEnd, line) == None:
              state = 8
        result = re.search(constantMatch, line)
        if result != None:
          c = Constant(result.groups(0)[0], result.groups(0)[1], result.groups(0)[2], comment)
          comment = ''
          o.addConstant(c)
        if len(objects) and o.mType == 'interface':
          result = re.search(interfaceMethodStart, line)
        else:
          result = re.search(methodStart, line)
        if result != None:
          methodName = result.groups(0)[2]
          if result.groups(0)[0] == 0:
            methodType = result.groups(0)[1]
          else:
            methodType = result.groups(0)[0] + ' ' + result.groups(0)[1]
          result = re.findall(parameter, line)
          if result != None:
            parameters = result
            if re.search(methodEnd, line) != None:
              m = Method(methodType, methodName, parameters, comment)
              comment = '';
              if inSubObject == 1:
                objects[0].addSubObjectMethod(m)
              else:
                objects[0].addMethod(m)
              if line.find(" override ") >= 0:
                m.setOverride()
              if line.find("throws ") < 0:
                state = 1
              else:
                throwsLine = line[line.find("throws ") + 7:]
                throwsLine = throwsLine.replace(" override", "")
                result = re.findall(typeList, throwsLine)
                m.addThrows(result);
                if re.search(typeListEnd, line) != None:
                  state = 1
                else:
                  state = 7
            else:
              state = 5
        result = re.search(staticMethodStart, line)
        if result != None:
          methodName = result.groups(0)[2]
          if result.groups(0)[0] == 0:
            methodType = result.groups(0)[1]
          else:
            methodType = result.groups(0)[0] + ' ' + result.groups(0)[1]
          result = re.findall(parameter, line)
          if result != None:
            parameters = result
            if re.search(methodEnd, line) != None:
              m = Method(methodType, methodName, parameters, comment)
              comment = '';
              if inSubObject == 1:
                objects[0].addSubObjectStaticMethod(m)
              else:
                objects[0].addStaticMethod(m)
              if line.find(" override ") >= 0:
                m.setOverride()
              if line.find("throws ") < 0:
                state = 1
              else:
                throwsLine = line[line.find("throws ") + 7:]
                throwsLine = throwsLine.replace(" override", "")
                result = re.findall(typeList, throwsLine)
                m.addThrows(result);
                if re.search(typeListEnd, line) != None:
                  state = 1
                else:
                  state = 7
            else:
              state = 6
        result = re.search(fieldMatch, line)
        if result != None:
          fieldKind = result.groups(0)[0]
          if result.groups(0)[1] == 0:
            fieldType = result.groups(0)[2]
          else:
            fieldType = result.groups(0)[1] + ' ' + result.groups(0)[2]
          fieldName = result.groups(0)[3]
          field = Field(fieldType, fieldName, comment)
          comment = ''
          if inSubObject == 1:
            objects[0].addSubObjectReceivedField(field)
          else:
            objects[0].addReceivedField(field)
          state = 1

      elif state == 5:
        result = re.findall(parameter, line)
        parameters += result
        if re.search(methodEnd, line) != None:
          if inSubObject == 1:
            objects[0].addSubObjectMethod(m)
          else:
            objects[0].addMethod(m)
          state = 1
      elif state == 6:
        result = re.findall(parameter, line)
        parameters += result
        if re.search(methodEnd, line) != None:
          if inSubObject == 1:
            objects[0].addSubObjectStaticMethod(m)
          else:
            objects[0].addStaticMethod(m)
          state = 1
      elif state == 7:
        result = re.findall(typeList, line)
        m.addThrows(result);
        if line.find(" override ") >= 0:
          m.setOverride()
        if re.search(typeListEnd, line) != None:
          state = 1
      elif state == 9:
        result = re.findall(typeList, line)
        o.addImplements(result);
        if re.search(typeListEnd, line) != None:
          state = 1

  f_in.close()

  return objects;

def walkDir(path):
  from os import walk

  print ("walking " + path)
  from os import listdir
  from os.path import isfile, join
  files = []
  dirs = []
  objects = []
  for (dirpath, dirnames, filenames) in walk(path):
    files.extend(filenames)
    dirs.extend(dirnames)
    break
  
  for f in files:
    objects.extend(readFile(join(path, f)))

  for d in dirs:
    objects.extend(walkDir(join(path, d)))

  return objects;


def printDoc(objects, destinationDir, dictionary):
  for o in objects:
    o.printDoc(destinationDir, dictionary)
  
  from os.path import isfile, join
  fileName = join(destinationDir, "index.html")
  file = open(fileName, "w")
  print fileName
  package = ''
  file.write('<html>\n')
  file.write('<head>\n')
  file.write('<link href="wiseydoc.css" rel="stylesheet" type="text/css">\n')
  file.write('</head>\n')
  file.write('<body>\n')
  file.write('  <div class="contents">\n')

  for o in objects:
    if o.mPackage != package:
      if len(package) > 0:
        file.write('      </tbody>\n')
        file.write('    </table>\n')

      file.write('    <table class="docSection">\n')
      file.write('      <tbody>\n')
      file.write('        <tr class="header">\n')
      file.write('          <td colspan="2">\n')
      file.write('            <h2 class="sectionHeader">Package ' + o.mPackage + '</h2>\n')
      file.write('          </td>\n')
      file.write('        </tr>\n')
      package = o.mPackage

    o.printShortDoc(file)

  file.write('  </div>\n')
  file.write('</body>\n')
  file.write('</html>\n')
  file.close()

def buildDictionary(objects):
  dictionary = {}
  for o in objects:
    dictionary[o.mName] = o
  return dictionary

def linkOverrides(objects, dictionary):
  for o in objects:
    for m in o.mMethods:
      if m.mIsOverride:
        for i in o.mImplements:
          if i in dictionary:
            interface = dictionary[i]
            for im in interface.mMethods:
              if m.mName == im.mName:
                m.mOverridenMethod = im
                m.mOverrideFromObject = interface

def main(argv):
  sourceDir = ''
  try:
    opts, args = getopt.getopt(argv,"s:d:",["source=","destination="])
  except getopt.GetoptError:
    print 'test.py -s <source directory> -d <destination directory>'
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print 'test.py -d <source file directory>'
      sys.exit()
    elif opt in ("-s", "--source"):
      sourceDir = arg
    elif opt in ("-d", "--destination"):
      destinationDir = arg
  print 'Source is ', sourceDir
  print 'Destination is ', destinationDir

  objects = walkDir(sourceDir)
  objects = sorted(objects, key=lambda o: o.mPackage + o.mName)
  dictionary = buildDictionary(objects)
  linkOverrides(objects, dictionary)
  printDoc(objects, destinationDir, dictionary)

if __name__ == "__main__":
   main(sys.argv[1:])
