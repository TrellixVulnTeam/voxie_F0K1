#!/usr/bin/python3
#
# Copyright (c) 2014-2022 The Voxie Authors
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import os
import sys
import subprocess
import io

import xml.etree.ElementTree

import property_types
import codegen_utils
import dbus_xml

os.chdir(os.path.join(os.path.dirname(sys.argv[0]), '..'))

# dbusXmlFilename = 'de.uni_stuttgart.Voxie.xml'
dbusXmlData = subprocess.run(['tools/get-dbus-xml.py'], check=True, stdout=subprocess.PIPE).stdout

typeListFilename = 'src/VoxieClient/DBusTypeList.cpp'
typeListHeaderFilename = 'src/VoxieClient/DBusTypeList.hpp'

types = set()
types.add('a{oo}')

# doc = xml.etree.ElementTree.parse(dbusXmlFilename)
doc = xml.etree.ElementTree.parse(io.BytesIO(dbusXmlData))
dbus_xml.iterTypes(doc=doc, showMissingOptions=False, typeCallback=types.add)
for prop in property_types.types:
    types.add(property_types.types[prop]['DBusSignature'])

todo = list(types)
while len(todo) != 0:
    type = todo[-1]
    todo.remove(type)
    for subType in property_types.dbusGetComponentTypes(type):
        if subType not in types:
            types.add(subType)
            todo.append(subType)

# Note: Types like 'a(dddd)' in src/Voxie/Node/Node.cpp does not seem to require a Q_DECLARE_METATYPE / qRegisterMetaType<>()

types = list(types)
types.sort()
# print(types)

with open(typeListFilename + '.new', 'w') as typeList, open(typeListHeaderFilename + '.new', 'w') as typeListHeader:
    typeListHeader.write(
        '// This file was automatically generated by tools/update-dbus-types.py\n')
    typeListHeader.write('#pragma once\n')
    typeListHeader.write('\n')
    typeListHeader.write('// All changes to this file will be lost\n')
    typeListHeader.write('\n')
    typeListHeader.write('#include <VoxieClient/VoxieClient.hpp>\n')
    typeListHeader.write('#include <VoxieClient/DBusTypes.hpp>\n')
    typeListHeader.write('\n')
    typeListHeader.write('namespace vx {')
    typeListHeader.write('VOXIECLIENT_EXPORT void initDBusTypes();')
    typeListHeader.write('}\n')
    typeListHeader.write('\n')

    typeList.write(
        '// This file was automatically generated by tools/update-dbus-types.py\n')
    typeList.write('// All changes to this file will be lost\n')
    typeList.write('\n')
    typeList.write('#include "DBusTypeList.hpp"\n')
    typeList.write('\n')
    typeList.write('#include <QtDBus/QDBusMetaType>\n')
    typeList.write('\n')
    typeList.write('void vx::initDBusTypes() {\n')

    for type in types:
        # print(type)
        qtType = dbus_xml.getQtType(type)
        if qtType is not None:
            qtType = dbus_xml.escapeQtType(qtType)
            typeListHeader.write('Q_DECLARE_METATYPE(' + qtType + ')\n')
            typeList.write('qDBusRegisterMetaType<' + qtType + '>();\n')

    typeList.write('}\n')

codegen_utils.formatAndRename(typeListFilename)
codegen_utils.formatAndRename(typeListHeaderFilename)
