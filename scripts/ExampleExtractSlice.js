/*
 * Copyright (c) 2014-2022 The Voxie Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

var size = [20, 5];

var client = voxie.CreateClient ()
try {
    //print (client)
    var dataSet = voxie.Gui.ActiveVisualizer.DataSet
    var image = voxie.CreateImage (client, size)
    dataSet.FilteredData.ExtractSlice ([0, 0, 0], [1, 0, 0, 0], size, [0.1, 0.1], image, { Interpolation: 'NearestNeighbor' })
    var str = '\n';
    for (var y = 0; y < image.Height; y++) {
        for (var x = 0; x < image.Width; x++) {
            str += image.GetPixel (x, y)
            str += ' '
        }
        str += '\n'
    }
    print (str)
} finally {
    voxie.DestroyClient (client)
}
