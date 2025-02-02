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

#include <PluginVis3D/Visualizer3DView.hpp>

#include <PluginVis3D/CamProperties.hpp>
#include <PluginVis3D/Data/AxisFilter.hpp>
#include <PluginVis3D/Helper/IsosurfaceMouseOperation.hpp>
#include <PluginVis3D/Helper/NotifyingLinkedList.hpp>
#include <PluginVis3D/PlanePropertiesUi.hpp>
#include <PluginVis3D/Prototypes.hpp>

#include <Voxie/Gui/ObjectProperties.hpp>

#include <VoxieBackend/Data/VolumeDataVoxel.hpp>

#include <Voxie/Vis/VisualizerNode.hpp>

#include <QtCore/QPointer>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

/**
 * @brief This class is the overall container for the operations related to the
 * isosurface. It basically contains the UI elements,
 * responds to user interaction and contains the @link Visualizer3DView which
 * again is responsible for the rendering of the 3D scene.
 */
class Visualizer3D : public vx::VisualizerNode {
  Q_OBJECT

  View3DProperties* properties;

  Visualizer3DView* view;
  PlanePropertiesUi* planeWidget;

  // Mouse Operation Widget:
  QPushButton* moveSurfaceButton;
  QPushButton* rotateSurfaceButton;
  QPushButton* selectSurfaceButton;
  QPushButton* addDataPointButton;
  QWidget* axisControlBox;
  QPushButton* filterXButton;
  QPushButton* filterYButton;
  QPushButton* filterZButton;
  // QPushButton* movePlaneButton;

  // Render Settings Widget
  QComboBox* culling;
  /**
   * @brief orthoProjectionBox Checkbox that indicates whether or not
   * orthographic projection is currently used
   */
  QCheckBox* orthoProjectionBox;

  QPointer<QObject> osvrDisplay = nullptr;

  const QSharedPointer<IsosurfaceMouseOperation> mouseOperation;

  AxisFilter axisFilter;
  bool movePlane;

 public:
  FACTORY_VISUALIZERMODULE_HPP(Isosurface)

  explicit Visualizer3D();

  QWidget* mainView() override { return view; }

  /**
   * @brief setCuttingMode sets the cutting mode and cutting limit for the
   given data set.
   * @param dataSet
   * @param mode
   * @param limit
   */
  void setCuttingMode(const int mode, const int limit) {
    this->view->cuttingMode.setMode((CuttingMode)mode, limit);
  }

 private:
  QWidget* createObjectWidget();
  QWidget* createMouseOperationWidget();

  /**
   * @brief settingUpCamProperties contains the name setting and connections
   * for CameraProperties
   * @param camProp
   */
  void settingUpCamProperties(CamProperties* camProp);

 private Q_SLOTS:
  void handleViewChange(QString direction);
  void preferedMouseActionChanged();
  /**
   * @brief updateOrthoBox Changes the status of the checkmark depending on
   * whether orthographic projection is on or off
   */
  void updateOrthoBox();

  void axisFilterChanged();

 public:
  vx::SharedFunPtr<RenderFunction> getRenderFunction() override;
};
