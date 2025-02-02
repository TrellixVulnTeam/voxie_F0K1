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

import sys
import os
import subprocess
import json
import io

import property_types
import codegen_utils

os.chdir(os.path.join(os.path.dirname(sys.argv[0]), '..'))

typeJsonFilename = 'pythonlib/voxie/PropertyTypes.json'
typeListFilename = 'src/Voxie/Node/Types.List.cpp'
typeListHeaderFilename = 'src/Voxie/Node/Types.List.hpp'

jsonData = {}
for typeName in property_types.types:
    type = dict(property_types.types[typeName])
    for key in ['ShortName', 'QtType', 'DefaultValueExpression', 'JSONParseFunction', 'VerifyFunction', 'CanonicalizeFunction']:
        if key in type:
            del type[key]
    jsonData[typeName] = type
f = io.StringIO()
json.dump(jsonData, f, allow_nan=False,
          sort_keys=True, ensure_ascii=False, indent=2)
s = bytes(f.getvalue() + '\n', 'utf-8')
with open(typeJsonFilename + '.new', 'wb') as typeJsonFile:
    typeJsonFile.write(s)
os.rename(typeJsonFilename + '.new', typeJsonFilename)


def escapeCppString(str):
    b = bytes(str, 'utf-8')
    s = '"'
    for c in b:
        if c >= 32 and c < 127 and c != '"':
            s += chr(c)
        else:
            s += '\\{0:03o}'.format(c)
    s += '"'
    return s


def escapeCppList(li):
    s = "{"
    for val in li:
        s += escapeCppString(val) + ", "
    s += "}"
    return s


with open(typeListFilename + '.new', 'w') as typeList, open(typeListHeaderFilename + '.new', 'w') as typeListHeader:
    typeList.write(
        '// This file was automatically generated by tools/update-property-types.py\n')
    typeList.write('// All changes to this file will be lost\n')
    typeList.write('\n')
    typeList.write('// This file will be included by Types.cpp\n')
    typeList.write('\n')

    typeListHeader.write(
        '// This file was automatically generated by tools/update-property-types.py\n')
    typeListHeader.write('// All changes to this file will be lost\n')
    typeListHeader.write('\n')
    typeListHeader.write('// This file will be included by Types.hpp\n')
    typeListHeader.write('\n')

    typeListHeader.write('namespace vx {')
    typeListHeader.write('namespace types {')

    keys = list(property_types.types)
    keys.sort()
    listOfTypes = ''
    for key in keys:
        ptype = property_types.types[key]

        name = key

        sname = name
        if '.' in sname:
            sname = sname[sname.rindex('.') + 1:]
        if 'ShortName' in ptype:
            sname = ptype['ShortName']

        # TODO: remove?
        typeListHeader.write(
            'VOXIECORESHARED_EXPORT QSharedPointer<PropertyType> %sType();\n' % (sname,))
    typeListHeader.write('\n')
    for key in keys:
        ptype = property_types.types[key]

        name = key

        sname = name
        if '.' in sname:
            sname = sname[sname.rindex('.') + 1:]
        if 'ShortName' in ptype:
            sname = ptype['ShortName']

        dname = ptype['DisplayName']

        dbusSig = ptype['DBusSignature']

        qtDBusType = property_types.dbusToCppRawType(dbusSig)
        rawType = qtDBusType
        if 'RawType' in ptype:
            rawType = ptype['RawType']

        qtType = qtDBusType
        if 'QtType' in ptype:
            qtType = ptype['QtType']

        defValExpr = ptype['DefaultValueExpression']

        compareSnippet = '''
  bool isComparable() override { return false; }
  int compare(/*const QSharedPointer<NodeProperty>&,*/ const QVariant&, const QVariant&) override { throw vx::Exception("de.uni_stuttgart.Voxie.InvalidOperation", "Comparison operator not implemented"); }
'''
        if 'CompareFunction' in ptype:
            compareFun = ptype['CompareFunction']
            compareSnippet = '''
  bool isComparable() override {{ return true; }}
  int compare(/*const QSharedPointer<NodeProperty>& property,*/ const QVariant& v1, const QVariant& v2) override {{
    if (v1.userType() != qMetaTypeId<{rawType}>())
      throw Exception(
          "de.uni_stuttgart.Voxie.InvalidPropertyValue",
          QString("Property value has unexpected type, expected %1, got %2 (%3)")
              .arg(QMetaType::typeName(qMetaTypeId<{rawType}>()))
              .arg(QMetaType::typeName(v1.userType()))
              .arg(v1.userType()));
    if (v2.userType() != qMetaTypeId<{rawType}>())
      throw Exception(
          "de.uni_stuttgart.Voxie.InvalidPropertyValue",
          QString("Property value has unexpected type, expected %1, got %2 (%3)")
              .arg(QMetaType::typeName(qMetaTypeId<{rawType}>()))
              .arg(QMetaType::typeName(v2.userType()))
              .arg(v2.userType()));
    return {compareFun}(// property,
                        vx::PropertyValueConvertRaw<{rawType}, {qtType}>::fromRaw(v1.value<{rawType}>()),
                        vx::PropertyValueConvertRaw<{rawType}, {qtType}>::fromRaw(v2.value<{rawType}>()));
  }}
''' \
            .format(**{
                'sname': sname,
                'compareFun': compareFun,
                'rawType': rawType,
                'qtType': qtType,
            })

        parseFun = 'ParseJsonFun<%s>::parse' % (rawType,)
        if 'JSONParseFunction' in ptype:
            if ptype['JSONParseFunction'] is None:
                parseFun = 'parseNotSupported<%s>' % (rawType,)
            else:
                parseFun = ptype['JSONParseFunction']

        compatNames = []
        if 'CompatibilityNames' in ptype:
            compatNames = ptype['CompatibilityNames']

        typeList.write('''
namespace {{
class PropertyType{sname} : public PropertyType {{
public:
        PropertyType{sname}() : PropertyType({name}, {displayName}, QVariant::fromValue<{RawType}>({defaultValue})) {{
  }}
  ~PropertyType{sname}() {{}}

  void verifyValue(NodeProperty &property,
                   const QVariant &value) override {{
    Q_UNUSED(property);
    if (value.userType() != qMetaTypeId<{RawType}>())
      throw Exception(
          "de.uni_stuttgart.Voxie.InvalidPropertyValue",
          QString("Property value has unexpected type, expected %1, got %2 (%3)")
              .arg(QMetaType::typeName(qMetaTypeId<{RawType}>()))
              .arg(QMetaType::typeName(value.userType()))
              .arg(value.userType()));
{verifyCode}
  }}
  QVariant canonicalize(NodeProperty &property,
                   const QVariant &value) override {{
{canonicalizeCode}
  }}
  int getRawQMetaType() override {{
    return qMetaTypeId<{RawType}>();
  }}

  {compareSnippet}

  PropertyUI *createUI(const QSharedPointer<NodeProperty> &property,
                       Node *node) override {{
    return new {sname}UI(property, node);
  }}

  QList<QString> compatibilityNames() override {{
    return {compatNames};
  }}
  QVariant parseJson(const QJsonValue &value) override {{
    return QVariant::fromValue<{RawType}>({parseFun}(value));
  }}
  QString valueToString(const QVariant &value) override {{
    return vx::valueToString<{RawType}>(value, this);
  }}
  QDBusVariant rawToDBus(const QVariant& value) override {{
    if (value.userType() != qMetaTypeId<{RawType}>())
      throw Exception(
          "de.uni_stuttgart.Voxie.InvalidPropertyValue",
          QString("Property value has unexpected type, expected %1, got %2 (%3)")
              .arg(QMetaType::typeName(qMetaTypeId<{RawType}>()))
              .arg(QMetaType::typeName(value.userType()))
              .arg(value.userType()));
    auto valueRaw = value.value<{RawType}>();
    return dbusMakeVariant<{DBusType}>(PropertyValueConvertDBus<{RawType}, {DBusType}>::fromRaw(valueRaw));
  }}
  QVariant dbusToRaw(const QDBusVariant& value) override {{
    return QVariant::fromValue<{RawType}>(PropertyValueConvertDBus<{RawType}, {DBusType}>::toRaw(dbusGetVariantValue<{DBusType}>(value)));
  }}

  QDBusSignature dbusSignature() override {{
    return QDBusSignature({dbusSig});
  }}
}};
}}
QSharedPointer<PropertyType> vx::types::{sname}Type() {{
  return vx::types::{sname}::type();
}}
QSharedPointer<PropertyType> vx::types::{sname}::type() {{
  static QSharedPointer<PropertyType> type = makeSharedQObject<PropertyType{sname}>();
  return type;
}}
'''.format(**{
            'sname': sname,
            'name': escapeCppString(name),
            'compatNames': escapeCppList(compatNames),
            'displayName': escapeCppString(dname),
            'dbusSig': escapeCppString(dbusSig),
            'DBusType': qtDBusType,
            'RawType': rawType,
            'defaultValue': defValExpr,
            'compareSnippet': compareSnippet,
            # TODO
            'verifyCode': ('{VerifyFunction}(property, value.value<{RawType}>());'.format(VerifyFunction=ptype['VerifyFunction'], RawType=rawType)) if 'VerifyFunction' in ptype else '',
            'canonicalizeCode': ('return QVariant::fromValue<{RawType}>({CanonicalizeFunction}(property, value.value<{RawType}>()));'.format(CanonicalizeFunction=ptype['CanonicalizeFunction'], RawType=rawType)) if 'CanonicalizeFunction' in ptype else 'verifyValue(property, value); return value;',
            'parseFun': parseFun,
        }))
        listOfTypes += 'vx::types::%s::type(), ' % (sname,)
        typeListHeader.write('''struct {sname} {{
  typedef {RawType} RawType;
  typedef {QtType} QtType;
  static VOXIECORESHARED_EXPORT QSharedPointer<PropertyType> type();
}};'''.format(
            sname=sname,
            RawType=rawType,
            QtType=qtType,
        ))
    typeList.write('\n')
    typeList.write('#define LIST_ALL_TYPES %s\n' % (listOfTypes,))
    typeListHeader.write('}')
    typeListHeader.write('}\n')

codegen_utils.formatAndRename(typeListFilename)
codegen_utils.formatAndRename(typeListHeaderFilename)
