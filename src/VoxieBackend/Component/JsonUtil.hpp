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

#pragma once

#include <VoxieBackend/VoxieBackend.hpp>

#include <QtCore/QJsonDocument>

namespace vx {
// filename is for diagnostic purposes
VOXIEBACKEND_EXPORT QJsonDocument parseJsonData(const QByteArray& data,
                                                const QString& filename);

VOXIEBACKEND_EXPORT QJsonDocument parseJsonFile(const QString& filename);

VOXIEBACKEND_EXPORT QString jsonTypeToString(QJsonValue::Type type);

VOXIEBACKEND_EXPORT QString expectString(const QJsonValue& value);
VOXIEBACKEND_EXPORT QJsonObject expectObject(const QJsonValue& value);
VOXIEBACKEND_EXPORT QJsonArray expectArray(const QJsonValue& value);

VOXIEBACKEND_EXPORT QJsonObject expectObject(const QJsonDocument& doc);
VOXIEBACKEND_EXPORT QJsonArray expectArray(const QJsonDocument& doc);
}  // namespace vx
