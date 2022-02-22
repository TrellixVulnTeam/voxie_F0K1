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
import voxie, dbus
instance = voxie.instanceFromArgs()


### de.uni_stuttgart.Voxie.ObjectKind.Data ###
# de.uni_stuttgart.Voxie.GeometricPrimitiveObject #
o_GeometricPrimitiveObject_1 = instance.CreateObjectChecked('de.uni_stuttgart.Voxie.GeometricPrimitiveObject', {
})

gpd = o_GeometricPrimitiveObject_1.CastTo('de.uni_stuttgart.Voxie.DataObject').Data.CastTo('de.uni_stuttgart.Voxie.GeometricPrimitiveData')
pointType = instance.Components.GetComponent('de.uni_stuttgart.Voxie.ComponentType.GeometricPrimitiveType', 'de.uni_stuttgart.Voxie.GeometricPrimitive.Point').CastTo('de.uni_stuttgart.Voxie.GeometricPrimitiveType')
with gpd.CreateUpdate() as gpdu:
    gpd.AddPrimitive(gpdu, pointType, 'Point1', { 'Position': voxie.Variant('(ddd)', ( 0.03, 0.03, 0.0 ))})
    gpd.AddPrimitive(gpdu, pointType, 'Point2', { 'Position': voxie.Variant('(ddd)', ( -0.03, -0.03, 0.0 ))})
    gpdu.Finish()

# de.uni_stuttgart.Voxie.VolumeObject #
o_VolumeObject_1 = instance.OpenFileChecked('/home/kiesssn/files/stuff/rpi-2.hdf5') # 'de.uni_stuttgart.Voxie.VolumeObject'
o_VolumeObject_1.SetPropertiesChecked({
  'de.uni_stuttgart.Voxie.MovableDataObject.Rotation': voxie.Variant('(dddd)', (1.0, 0.0, 0.0, 0.0)),
  'de.uni_stuttgart.Voxie.MovableDataObject.Translation': voxie.Variant('(ddd)', (0.0, 0.0, 0.0)),
})

### de.uni_stuttgart.Voxie.ObjectKind.Visualizer ###
# de.uni_stuttgart.Voxie.SliceVisualizer #
o_SliceVisualizer_1 = instance.CreateObjectChecked('de.uni_stuttgart.Voxie.SliceVisualizer', {
  'de.uni_stuttgart.Voxie.View2D.CenterPoint': voxie.Variant('(dd)', (0.0, 0.0)),
  'de.uni_stuttgart.Voxie.SliceVisualizer.GeometricPrimitive': voxie.Variant('o', o_GeometricPrimitiveObject_1),
  'de.uni_stuttgart.Voxie.SliceVisualizer.Interpolation': voxie.Variant('s', 'de.uni_stuttgart.Voxie.Interpolation.Linear'),
  'de.uni_stuttgart.Voxie.SliceVisualizer.Plane': voxie.Variant('o', None),
  'de.uni_stuttgart.Voxie.SliceVisualizer.Plane.Orientation': voxie.Variant('(dddd)', (1.0, 0.0, 0.0, 0.0)),
  'de.uni_stuttgart.Voxie.SliceVisualizer.Plane.Origin': voxie.Variant('(ddd)', (-3.725290298461914e-09, 0.00035387277603149414, -3.725290298461914e-09)),
  'de.uni_stuttgart.Voxie.SliceVisualizer.ShowSliceCenter': voxie.Variant('b', True),
  'de.uni_stuttgart.Voxie.SliceVisualizer.ValueColorMapping': voxie.Variant('a(d(dddd))', [(float('NaN'), (0.0, 0.0, 0.0, 0.0)), (0.0, (0.0, 0.0, 0.0, 1.0)), (1.0, (1.0, 1.0, 1.0, 1.0))]),
  'de.uni_stuttgart.Voxie.View2D.VerticalSize': voxie.Variant('d', 0.1),
  'de.uni_stuttgart.Voxie.SliceVisualizer.Volume': voxie.Variant('o', o_VolumeObject_1),
})
