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

// This file was automatically generated by tools/update-dbus-types.py
// All changes to this file will be lost

#include "DBusTypeList.hpp"

#include <QtDBus/QDBusMetaType>

void vx::initDBusTypes() {
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<std::tuple<double, double, double>,
                  std::tuple<double, double, double>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<std::tuple<double, double, double>, double>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<QMap<QString, QDBusVariant>, qint64,
                  std::tuple<QString, quint32, QString>, std::tuple<quint64>,
                  std::tuple<qint64>, QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<QMap<QString, QDBusVariant>, qint64,
                  std::tuple<QString, quint32, QString>,
                  std::tuple<quint64, quint64>, std::tuple<qint64, qint64>,
                  QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<QMap<QString, QDBusVariant>, qint64,
                  std::tuple<QString, quint32, QString>,
                  std::tuple<quint64, quint64, quint64>,
                  std::tuple<qint64, qint64, qint64>,
                  QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((std::tuple<bool, QString>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<double, std::tuple<double, double, double, double>,
                  qint32>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<double, std::tuple<double, double, double, double>,
                  qint64>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((std::tuple<double, double>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<double, double, double>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<double, double, double, double>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((
      std::tuple<QString, std::tuple<QString, quint32, QString>, QString,
                 QMap<QString, QDBusVariant>, QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<QString, QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<QString, QDBusObjectPath>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((
      std::tuple<QString, QDBusObjectPath, QString, QMap<QString, QDBusVariant>,
                 QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((
      std::tuple<QString, QString, quint64,
                 std::tuple<QString, quint32, QString>, QString,
                 QMap<QString, QDBusVariant>, QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((std::tuple<QString, quint64>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<QString, quint32, QString>))>();
  qDBusRegisterMetaType<std::tuple<quint64>>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<quint64, QList<QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((
      std::tuple<quint64, QDBusObjectPath, QString, QMap<QString, QDBusVariant>,
                 QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((std::tuple<quint64, quint64>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<quint64, quint64, quint64>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<quint32, quint32, quint32>))>();
  qDBusRegisterMetaType<std::tuple<qint64>>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((std::tuple<qint64, qint64>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (std::tuple<qint64, qint64, qint64>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<std::tuple<double, double, double>, double>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<double, std::tuple<double, double, double, double>,
                        qint32>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<double, std::tuple<double, double, double, double>,
                        qint64>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<double, double, double>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<QString, std::tuple<QString, quint32, QString>, QString,
                        QMap<QString, QDBusVariant>,
                        QMap<QString, QDBusVariant>>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<QString, QDBusObjectPath, QString,
                        QMap<QString, QDBusVariant>,
                        QMap<QString, QDBusVariant>>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<QString, QString, quint64,
                        std::tuple<QString, quint32, QString>, QString,
                        QMap<QString, QDBusVariant>,
                        QMap<QString, QDBusVariant>>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<QString, quint64>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<quint64, QList<QDBusVariant>>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<quint64, QDBusObjectPath, QString,
                        QMap<QString, QDBusVariant>,
                        QMap<QString, QDBusVariant>>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<std::tuple<quint32, quint32, quint32>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QList<QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<QList<quint64>>();
  qDBusRegisterMetaType<QList<QDBusVariant>>();
  qDBusRegisterMetaType<QList<qint64>>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QMap<QDBusObjectPath, QMap<QString, QDBusVariant>>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE(
      (QMap<QDBusObjectPath, QDBusObjectPath>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((QMap<QDBusObjectPath, quint64>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((QMap<QString, QDBusSignature>))>();
  qDBusRegisterMetaType<VX_IDENTITY_TYPE((QMap<QString, QDBusVariant>))>();
}
