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

#include <VoxieBackend/Data/InterpolationMethod.hpp>

#include <QtWidgets/QComboBox>

#include <PluginVisSlice/BrushSelectionTool.hpp>
#include <PluginVisSlice/ImageGeneratorWorker.hpp>
#include <PluginVisSlice/LassoSelectionTool.hpp>
#include <PluginVisSlice/Prototypes.forward.hpp>
#include <PluginVisSlice/Prototypes.hpp>  // TODO: Should not be needed here, move stuff out of header
#include <PluginVisSlice/ToolSelection.hpp>
#include <PluginVisSlice/Visualizer2DTool.hpp>

#include <PluginVisSlice/GeometricAnalysisTool.hpp>
#include <PluginVisSlice/Grid.hpp>
#include <PluginVisSlice/ImageLayer.hpp>
#include <PluginVisSlice/LabelLayer.hpp>
#include <PluginVisSlice/SelectionLayer.hpp>

#include <QRadioButton>
#include <Voxie/Data/InitializeColorizeWorker.hpp>
#include <Voxie/Data/Slice.hpp>
#include <Voxie/PropertyObjects/PlaneNode.hpp>
#include <VoxieBackend/Data/SliceImage.hpp>

#include <Voxie/Interfaces/SliceVisualizerI.hpp>

#include <Voxie/Vis/FilterChain2DWidget.hpp>
#include <Voxie/Vis/HistogramWidget.hpp>
#include <Voxie/Vis/VisualizerNode.hpp>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QVarLengthArray>
#include <QtCore/QVector>

#include <QtGui/QPaintEvent>

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

class ImagePaintWidget;

/**
 * @brief The SliceVisualizer class is a godclass mainly responsible for
 * initializing and handling the data flow between all slice view operations.
 * @author Hans Martin Berner, David Haegele
 */

namespace vx {
class ColorizerEntry;
class HistogramProvider;
class ParameterCopy;
}  // namespace vx

class InfoWidget;
class ImageLayer;

class SliceVisualizer : public vx::VisualizerNode, public vx::SliceVisualizerI {
  friend class ImagePaintWidget;
  Q_OBJECT

 public:
  vx::SliceProperties* properties;

  const QString imageLayerName = "ImageLayer";
  const QString lassoLayerName = "LassoLayer";
  const QString brushLayerName = "BrushLayer";
  const QString brushSelectionName = "BrushSelectionTool";
  const QString lassoSelectionName = "LassoSelectionTool";

 private:  // magic numbers
  int RESIZE_TIMER_TIMEOUT = 100;

  QWidget* view;
  QComboBox* box;

  // sidebar widgets
  ImagePaintWidget* _imageDisplayingWidget;
  vx::visualization::FilterChain2DWidget* _filterChain2DWidget;

  QWidget* histogramBox;
  QRadioButton* sliceRadioButton;
  QRadioButton* volumeRadioButton;
  vx::HistogramWidget* _histogramWidget;

  InfoWidget* infoWidget;
  // data
  vx::VolumeNode* mainVolumeNode = nullptr;
  vx::PlaneNode* planeProperty = nullptr;
  vx::Slice* _slice = nullptr;
  vx::SliceImage _sliceImage;
  vx::SliceImage _filteredSliceImage;

  QSharedPointer<vx::HistogramProvider> sliceHistogramProvider;
  QSharedPointer<vx::HistogramProvider> histogramProvider;
  QMetaObject::Connection histogramConnection;

  // TODO: clean up
  QMap<int, QImage> _drawStack;

  // for window elements
  QVBoxLayout* hobox;
  QWidget* toolBar;
  QVarLengthArray<Visualizer2DTool*> _tools;
  QList<QSharedPointer<Layer>> layers_;

  int _currentTool =
      0;  // init value is tool to be activated when window is created

  vx::BrushSelectionTool* brushSelectionTool = nullptr;
  Visualizer2DTool* sliceAdjustmentTool = nullptr;
  vx::BrushSelectionLayer* brushSelectionLayer = nullptr;

  vx::LassoSelectionTool* lassoSelectionTool = nullptr;
  vx::LassoSelectionLayer* lassoSelectionLayer = nullptr;

  vx::InitializeColorizeWorker* initializeWorker;

  /**
   * When a change of the Plane is based by a PlaneNode change, don't
   * propagate it back to the PlaneNode
   */
  bool propagateToPlaneNode = true;

  /**
   * Handles resizes. We don't want to regenerate an image on each resize event
   * as this would need unnecessary amount of resources
   */
  QTimer _resizeTimer;

  /**
   * Makes sure that only one worker thread is running at a time.
   */
  bool _imageWorkerRunning = false;

  /**
   * The current worker that was queued up should another worker be running
   * already. This makes sure that we have always the latest request in the
   * queued up.
   */
  ImageGeneratorWorker* _imageQueueWorker = nullptr;

  bool _filterWorkerRunning = false;
  bool _filterImageWorked = false;

  vx::SliceImage _filterQueueImage;

  /**
   * selection tool
   */
  ToolSelection* selectionTool;

  void doGenerateSliceImage(vx::Slice* slice, const QRectF& sliceArea,
                            const QSize& imageSize,
                            vx::InterpolationMethod interpolation =
                                vx::InterpolationMethod::NearestNeighbor);
  void runSliceImageGeneratorWorker();
  void runFilterWorker();

  /**
   * @brief initialize Initializes the SliceVisualizer when a VolumeNode is
   * connected.
   */
  void initializeSV();

  QAction* createPlaneNodeAction;

  // Override needed for SegmentationFilter
  bool isAllowedParent(vx::NodeKind object) override;

 protected:
  QWidget* getCustomPropertySectionContent(const QString& name) override;

 public:
  explicit SliceVisualizer();
  FACTORY_VISUALIZERMODULE_HPP(Slice)

  QWidget* mainView() override { return view; }

  /**
   * @brief Appends all relevant segmentation functionality to the
   * SliceVisualizer: SelectionLayer, LabelLayer, BrushSelectionLayer &
   * BrushSelectionTool
   * @param brushRadius Radius of the brush in pixels
   */
  void addSegmentationFunctionality(quint8 brushRadius);

  /**
   * @brief Sets the brush selection tool as currently active tool
   */
  void activateBrushSelectionTool() override;

  /**
   * @brief Sets the SliceAdjustment tool as currently active tool
   */
  void deactivateBrushSelectionTool() override;

  /**
   * @brief Sets the lasso selection tool as currently active tool
   */
  void activateLassoSelectionTool() override;

  /**
   * @brief Set the current tool to SliceAdjustment if LassoSelection is active
   */
  void deactivateLassoSelectionTool() override;

  /**
   * @brief Sets the brush Radius in pixels
   */
  void setBrushRadius(quint8 radius) override;

  /**
   * @brief returns the current Brush radius in pixels
   */
  quint8 getBrushRadius() override;

  /**
   * @brief Returns the histogram provider of slice visualizer histogram widget
   */
  virtual QSharedPointer<vx::HistogramProvider> getSliceHistogramProvider()
      override;

  /**
   * @brief Sets the histogram provider of slice visualizer histogram widget
   */
  virtual void setHistogramColorizer(
      QSharedPointer<vx::Colorizer> colorizer) override;

  /**
   * @brief Get the colorizer of slice visualizer histogram widget
   */
  virtual QSharedPointer<vx::Colorizer> getHistogramColorizer() override;

  /**
   * @brief Set the volume property
   */
  void setVolume(vx::Node* volume) override;

 private:
  /**
   * Notifies the widget that draws the canvas that it needs to redraw the draw
   * stack.
   */
  void redraw();

 public:
  /**
   * @return the current mainVolumeNode
   */
  vx::VolumeNode* dataSet() { return this->mainVolumeNode; }

  /**
   * @return the current unfiltered slice image
   */
  vx::SliceImage& sliceImage() { return this->_sliceImage; }

  /**
   * @return the current filtered slice image
   */
  vx::SliceImage& filteredSliceImage() { return this->_filteredSliceImage; }

  /**
   * Notifies the tool that it has been activated. Notifies the previous tool
   * that it has been deactivated. This tool now gets forwarded all key and
   * mouse events.
   * @param tool pointer to the tool.
   */
  void switchToolTo(Visualizer2DTool* tool);

  /**
   * Returns the currently selected tool.
   * @see switchToolTo(Visualizer2DTool *tool);
   */
  Visualizer2DTool* currentTool() { return _tools.at(_currentTool); }

 private:
  /**
   * Add an image to the draw stack.
   * @param self pointer to the object the image is associated with
   * @param image to be added or replaced
   */
  void addToDrawStack(Layer* self, const QImage& image);

 public:
  /**
   * @return the current drawStack consisting of a mapping of index to image.
   * Smallest index is at the bottom. Begins at -1 (as >= 0 are tools).
   */
  QMap<int, QImage> drawStack() { return _drawStack; }

  int getLayerIndexByName(QString name);

  /**
   * @return array of tools added to the visualizer
   */
  QVarLengthArray<Visualizer2DTool*> tools() { return _tools; }

  QSharedPointer<Layer> imageLayer;

  const QList<QSharedPointer<Layer>>& layers() { return layers_; }

  static double getCurrentPixelSize(vx::SlicePropertiesBase* properties,
                                    const QSize& canvasSize);

  /**
   * @return the current plane area describing the current position and size of
   * the sliceimage inside the visualizer
   */
  static QRectF getCurrentPlaneArea(vx::SlicePropertiesBase* properties,
                                    const QSize& canvasSize);
  QRectF currentPlaneArea();

  /**
   * @return the slice associated with this visualizer. This pointer originates
   * from outside the plugin.
   */
  virtual vx::Slice* slice() final { return this->_slice; }

  /**
   * Resets the current plane area to the bounding rectangle putting it into the
   * upper left corner.
   */
  void resetPlaneArea() {
    //_currentPlaneArea = slice()->getBoundingRectangle();
    QRectF bbox = slice()->getBoundingRectangle();
    this->properties->setCenterPoint(bbox.center());
    this->properties->setVerticalSize(
        bbox.height() *
        1.1);  // TODO: Use diagonal though entire volume instead?
  }

  /**
   * @param multiplier zooms the plane area by a given delta. Multiplier is
   * multiplied with current zoom. e.g.
   */
  void zoomPlaneArea(qreal multiplier) {
    //_currentPlaneArea = zoomedArea(_currentPlaneArea, multiplier);
    this->properties->setVerticalSize(this->properties->verticalSize() /
                                      multiplier);
  }

  /**
   * Moves the current plane (representing the position of the image inside the
   * canvas) by the given values
   * @param pixelDeltaX delta x
   * @param pixelDeltaX delta y
   */
  void moveArea(qreal pixelDeltaX, qreal pixelDeltaY) {
    qreal relx = pixelDeltaX / this->canvasWidth();
    qreal rely = pixelDeltaY / this->canvasHeight();
    relx *= currentPlaneArea().width();
    rely *= currentPlaneArea().height();
    //_currentPlaneArea.translate(-relx, -rely);
    this->properties->setCenterPoint(this->properties->centerPoint() -
                                     QPointF(relx, rely));
  }

  /**
   * @return the canvas height representing the whole drawing area reserved for
   * the slice. This does not include borders or tools.
   */
  int canvasHeight() { return canvasSize().height(); }

  /**
   * @return the canvas width representing the whole drawing area reserved for
   * the slice. This does not include borders or tools.
   */
  int canvasWidth() { return canvasSize().width(); }

  /**
   * @return The size of the area reserved for displaying the rendered image
   * (not the window, not the toolbar)
   */
  QSize canvasSize();

  static QRectF adjustedAreaAspect(const QRectF& area, const QSize& oldSize,
                                   const QSize& newSize);

  static QRectF zoomedArea(const QRectF& area, qreal zoom);

  /**
   * @brief rotationChanged is called when the rotation of the plane is changed
   * by the user.
   * @param rotation is the new rotation of the plane
   */
  void rotationChanged(QQuaternion rotation);

  /**
   * @brief originChanged is called when the origin of the plane is changed by
   * the user.
   * @param origin is the new origin of the plane
   */
  void originChanged(QVector3D origin);

  void setRotation(QQuaternion rotation) override {
    if (this->slice()) {
      this->slice()->setRotation(rotation);
    }
  }

  void setOrigin(QVector3D origin) {
    if (this->slice()) {
      this->slice()->setOrigin(origin);
    }
  }

  QVariant getNodePropertyCustom(QString key) override;
  void setNodePropertyCustom(QString key, QVariant value) override;

  QSharedPointer<QObject> getPropertyUIData(QString propertyName) override;

  static vx::InterpolationMethod getInterpolation(
      vx::SlicePropertiesBase* properties);

  // Should be able to run in a background thread
  void renderEverything(QImage& outputImage,
                        const QSharedPointer<vx::ParameterCopy>& parameters);

  vx::SharedFunPtr<RenderFunction> getRenderFunction() override;

 public Q_SLOTS:
  /**
   * @brief onSliceImageGenerated is called when a slice image has been
   * generated from a slice (step 1)
   * @param image
   */
  void onSliceImageGenerated(vx::SliceImage image);
  /**
   * @brief onSliceImageFiltered is called when an image has been filtered (step
   * 2)
   * @param image
   */
  void onSliceImageFiltered(vx::SliceImage image);
  /**
   * @brief onSliceChanged is called when the slice changes
   * @param slice
   * @param oldPlane
   * @param newPlane
   * @param equivalent
   */
  void onSliceChanged(const vx::Slice* slice, const vx::PlaneInfo& oldPlane,
                      const vx::PlaneInfo& newPlane, bool equivalent);
  /**
   * @brief onDatasetChanged is called when the dataset associated with the
   * slice changes
   */
  void onDatasetChanged();
  /**
   * @brief applyFilters is called when the threaded filter call should be
   * invoked. It clones the currently generated sliceimage.
   */
  void applyFilters();
  /**
   * @brief updateSliceImageFromSlice is called when the sliceimage should be
   * generated (for step 1)
   */
  void updateSliceImageFromSlice();
  /**
   * @brief onFilterMaskRequest is called when the filterchain2d widget
   * (section) requests for the filter mask to be initialized. This is necessary
   * because the pointer is normally not known outside of this plugin.
   * @param mask pointer to the selection mask
   */
  void onFilterMaskRequest(vx::filter::Filter2D* mask);
  /**
   * @brief onCanvasResized is called when the ImagePaintWidget is resized
   */
  void onCanvasResized(QResizeEvent*);

  void initializeFinished(QVector<float> values);

  /**
   * Create a separate plane property node.
   */
  void createPlaneNode();
  void updateCreatePlaneNode();

 public:
  static QPointF planePointToPixel(const QSize& imageSize,
                                   const QRectF& planeArea,
                                   const QPointF& planePoint);

 Q_SIGNALS:
  // for data flow
  void signalBaseImageChanged(QImage baseImage);
  void signalRequestSliceImageUpdate();
  void resized();
  void signalRequestHistogram(vx::SliceImage& image);

  void imageMouseMove(QMouseEvent* e, const QPointF& pointPlane,
                      const QVector3D& threeDPoint, double valUnf,
                      double valFilt, double valNearest, double valLinear);

  // Signals forwarded from GeometricPrimitiveNode
  void newMeasurement();
  void currentPointChanged();
  void gpoDataChangedFinished();

  // Signals forwarded from PlaneNode
  void rotationChangedForward();
  void originChangedForward();

  // Signals forwarded from VolumeData / VolumeNode
  void volumeDataChangedFinished();
  void volumeDataRotationChanged();
  void volumeDataTranslationChanged();

  // Signals forwarded from LabelContainer
  void labelContainerChangedFinished();
};
