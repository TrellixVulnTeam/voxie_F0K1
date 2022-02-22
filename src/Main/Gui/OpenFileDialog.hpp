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

#include <QtWidgets/QFileDialog>

#include <QtCore/QMap>

namespace vx {
class Root;
class Node;
namespace io {
class Importer;
}

class OpenFileDialog : public QFileDialog {
  Q_OBJECT

  Root* root;
  QString supportedFilter;
  QMap<QString, QSharedPointer<vx::io::Importer>> map;
  QList<QSharedPointer<vx::io::Importer>> importers;

  QSharedPointer<vx::io::Importer> currentImporter;

  QMap<QSharedPointer<vx::io::Importer>, QSharedPointer<vx::Node>> fakeNodes;

  QList<QWidget*> additionalWidgets;

 public:
  OpenFileDialog(Root* root, QWidget* parent);
  ~OpenFileDialog();

 private:
  void finish(int result);

  QSharedPointer<vx::io::Importer> findImporter(const QString& filename);
  void updateImporter();
  QSharedPointer<vx::Node> getFakeNode(
      const QSharedPointer<vx::io::Importer>& importer);
  void setImporter(const QSharedPointer<vx::io::Importer>& importer);
};

}  // namespace vx
