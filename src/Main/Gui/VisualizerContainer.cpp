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

#include "VisualizerContainer.hpp"

#include <Voxie/Vis/VisualizerNode.hpp>

#include <QtCore/QDebug>
#include <QtCore/QEvent>

#include <QtGui/QCloseEvent>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>

using namespace vx;

VisualizerContainer::VisualizerContainer(QMdiArea* container,
                                         VisualizerNode* visualizer)
    : QWidget(nullptr),
      icon(visualizer->icon()),
      visualizer(visualizer),
      container(container),
      window(nullptr) {
  visualizer->setParent(this);
  this->setWindowTitle(this->visualizer->mainView()->windowTitle());
  connect(this->visualizer->mainView(), &QWidget::windowTitleChanged, this,
          &QWidget::setWindowTitle);
  this->setWindowIcon(icon);
  {
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    {
      QMenuBar* bar = new QMenuBar();
      bar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

      QAction* popOutAction = bar->addAction("Detach");
      connect(popOutAction, &QAction::triggered, this,
              &VisualizerContainer::switchPopState);

      QAction* selectNodeAction = bar->addAction("Select node");
      connect(selectNodeAction, &QAction::triggered, this, [this]() {
        QList<vx::Node*> nodes;
        nodes << this->visualizer;
        vx::voxieRoot().activeVisualizerProvider()->setSelectedNodes(nodes);
      });

      layout->addWidget(bar);
      layout->addWidget(visualizer->mainView());
    }
    this->setLayout(layout);
  }

  this->moveToNewMdiChild();
}

VisualizerContainer::~VisualizerContainer() {
  // qDebug() << "delete visContainer";
  if (this->window != nullptr) {
    delete this->window;
  }
}

void VisualizerContainer::changeEvent(QEvent* event) {
  if (this->window != nullptr) {
    return;
  }
  if ((event->type() != QEvent::WindowStateChange) &&
      (event->type() != QEvent::ActivationChange)) {
    return;
  }
  Q_EMIT this->sidePanelVisiblityChanged(this->isActiveWindow());
}

void VisualizerContainer::subWindowChanged(Qt::WindowStates oldState,
                                           Qt::WindowStates newState) {
  bool prev = (oldState & Qt::WindowActive) != 0;
  bool curr = (newState & Qt::WindowActive) != 0;

  if (prev != curr) {
    Q_EMIT this->sidePanelVisiblityChanged(curr);
  }
}

static void deleteObj(QMdiSubWindow** ref) {
  auto tmp = *ref;
  *ref = nullptr;
  delete tmp;
}

void VisualizerContainer::switchPopState() {
  if (this->window != nullptr) {
    for (QAction* action : this->window->findChildren<QAction*>()) {
      if (action->text() == "Detach") {
        action->setText("Attach");
      }
    }

    this->setParent(nullptr);
    deleteObj(&this->window);
    this->show();
    isAttached = false;
  } else {
    this->moveToNewMdiChild();
    isAttached = true;
  }
}

void VisualizerContainer::moveToNewMdiChild() {
  if (this->window != nullptr) {
    this->setParent(nullptr);
    deleteObj(&this->window);
  }

  this->window = this->container->addSubWindow(this);
  this->window->move(10, 10);
  this->window->setWindowIcon(icon);
  this->window->show();

  connect(this->window, &QMdiSubWindow::destroyed, this,
          &VisualizerContainer::destroyme);

  this->container->setActiveSubWindow(this->window);

  connect(this->window, &QMdiSubWindow::windowStateChanged, this,
          &VisualizerContainer::subWindowChanged);

  this->sidePanelVisiblityChanged(true);

  for (QAction* action : this->window->findChildren<QAction*>()) {
    if (action->text() == "Attach") {
      action->setText("Detach");
    }
  }
}

void VisualizerContainer::destroyme(QObject*) {
  if (this->window != nullptr) {
    this->window = nullptr;
    this->deleteLater();
  }
}

void VisualizerContainer::activate() {
  if (this->window != nullptr) {
    this->container->setActiveSubWindow(this->window);
  } else {
    this->activateWindow();
  }
}

void VisualizerContainer::setWindowMode(vx::VisualizerWindowMode mode) {
  switch (mode) {
    case vx::VisualizerWindowMode::Normal:
      this->window->showNormal();
      break;
    case vx::VisualizerWindowMode::Minimize:
      this->window->showMinimized();
      break;
    case vx::VisualizerWindowMode::Maximize:
      this->window->showMaximized();
      break;
    case vx::VisualizerWindowMode::FullScreen:
      this->window->showFullScreen();
      break;
    default:
      this->window->showNormal();
  }
}

void VisualizerContainer::closeWindow() {
  if (this->window != nullptr) {
    this->window->close();
  } else {
    this->close();
  }
}

VisualizerNode* VisualizerContainer::getVisualizer() { return visualizer; }

void VisualizerContainer::closeEvent(QCloseEvent* ev) {
  ev->accept();
  if (this->parent() == nullptr) {
    // will not delete itself without parent
    this->deleteLater();
  }
}

QPoint VisualizerContainer::getVisualizerPosition() {
  if (this->window != nullptr) {
    return this->window->pos();
  } else {
    return this->pos();
  }
}

void VisualizerContainer::setVisualizerPosition(QPoint pos) {
  if (this->window != nullptr) {
    this->window->move(pos);
  } else {
    this->move(pos);
  }
}

QSize VisualizerContainer::getVisualizerSize() {
  if (this->window != nullptr) {
    return this->window->size();
  } else {
    return this->size();
  }
}

void VisualizerContainer::setVisualizerSize(QSize size) {
  if (this->window != nullptr) {
    this->window->resize(size);
  } else {
    this->resize(size);
  }
}
