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

#include <Voxie/Component/Interfaces.hpp>

#include <QtCore/QList>

#include <QtGui/QGenericPlugin>

/**
 * @brief The VisTablePlugin class specifies the plugin properties and
 * initializes its classes.
 */

class VisTablePlugin : public QGenericPlugin,
                       public vx::plugin::IUICommandPlugin,
                       public vx::plugin::IObjectPrototypePlugin {
  Q_OBJECT
  Q_INTERFACES(vx::plugin::IUICommandPlugin)
  Q_INTERFACES(vx::plugin::IObjectPrototypePlugin)

  Q_PLUGIN_METADATA(IID
                    "org.qt-project.Qt.QGenericPluginPrototypeInterface" FILE
                    "VisTable.json")

 public:
  VisTablePlugin(QObject* parent = nullptr);

  virtual QObject* create(const QString& key,
                          const QString& specification) override;

  virtual QList<QAction*> uiCommands() override;

  virtual QList<QSharedPointer<vx::NodePrototype>> objectPrototypes() override;
};
