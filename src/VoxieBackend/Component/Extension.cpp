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

#include "Extension.hpp"

#include <VoxieClient/DBusAdaptors.hpp>

#include <VoxieClient/ObjectExport/BusManager.hpp>

#include <VoxieBackend/Component/Component.hpp>
#include <VoxieBackend/Component/ComponentType.hpp>
#include <VoxieBackend/Component/ExtensionLauncher.hpp>
#include <VoxieBackend/Component/ExternalOperation.hpp>
#include <VoxieBackend/Component/JsonUtil.hpp>

#include <VoxieBackend/IO/Operation.hpp>

#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

using namespace vx;

namespace vx {
class ExtensionAdaptorImpl : public ExtensionAdaptor {
  Extension* object;

 public:
  ExtensionAdaptorImpl(Extension* object)
      : ExtensionAdaptor(object), object(object) {}
  ~ExtensionAdaptorImpl() override {}

  QString executableFilename() const override {
    try {
      return object->scriptFilename();
    } catch (vx::Exception& e) {
      e.handle(object);
      return "";
    }
  }
};

class DynamicObjectAdaptorImplExtension : public DynamicObjectAdaptor {
  Extension* object;

 public:
  DynamicObjectAdaptorImplExtension(Extension* object)
      : DynamicObjectAdaptor(object), object(object) {}
  ~DynamicObjectAdaptorImplExtension() override {}

  QStringList supportedInterfaces() const override {
    try {
      // return object->supportedDBusInterfaces();
      return {"de.uni_stuttgart.Voxie.Extension"};
    } catch (Exception& e) {
      e.handle(object);
      return {};
    }
  }
};
}  // namespace vx

QList<QSharedPointer<Extension>> Extension::loadFromDir(
    const QString& dir, const ComponentTypeList& typeList,
    const QSharedPointer<ExtensionLauncher>& extensionLauncher,
    const QSharedPointer<DBusService>& dbusService,
    const vx::SharedFunPtr<QList<QSharedPointer<PropertyBase>>(
        const QJsonObject&)>& parseProperties) {
  QList<QSharedPointer<Extension>> result;

  QDir scriptDir = QDir(dir);
  QStringList configs =
      scriptDir.entryList(QStringList("*.json"), QDir::Files | QDir::Readable);
  for (QString config : configs) {
    QString configFile = dir + "/" + config;
    // if (QFileInfo(configFile).isExecutable())
    //  continue;
    QString executable = configFile.mid(0, configFile.length() - 5);
    auto extension =
        Extension::create(executable, configFile, typeList, extensionLauncher,
                          dbusService, parseProperties);
    result.append(extension);
  }

  return result;
}

Extension::Extension(const QString& scriptFilename, const QString& jsonFilename,
                     const ComponentTypeList& typeList,
                     const QSharedPointer<ExtensionLauncher>& extensionLauncher,
                     const QSharedPointer<DBusService>& dbusService,
                     const vx::SharedFunPtr<QList<QSharedPointer<PropertyBase>>(
                         const QJsonObject&)>& parseProperties)
    : ComponentContainer("Extension"),
      scriptFilename_(scriptFilename),
      extensionLauncher(extensionLauncher),
      dbusService(dbusService),
      componentTypes_(typeList) {
  new DynamicObjectAdaptorImplExtension(this);
  new ExtensionAdaptorImpl(this);

  auto jsonDoc = parseJsonFile(jsonFilename);

  for (const auto& type : *typeList) {
    // Skip types which are not supported in extensions
    if (!type->parseFunction()) {
      components_[type->name()] = QList<QSharedPointer<Component>>();
      continue;
    }

    QList<QJsonValue> entries;
    for (const auto& entry : jsonDoc.object()[type->jsonName()].toArray())
      entries << entry;

    for (const auto& entry : type->compatibilityJsonNames()) {
      auto oldName = std::get<0>(entry);
      auto warn = std::get<1>(entry);
      if (!jsonDoc.object().contains(oldName)) continue;

      if (warn)
        qWarning() << "Extension file" << jsonFilename << "contains deprecated"
                   << oldName
                   << "section, should be "
                      "renamed to"
                   << type->jsonName();

      for (const auto& entry2 : jsonDoc.object()[oldName].toArray())
        entries << entry2;
    }

    QList<QSharedPointer<Component>> components;
    for (const auto& entry : entries) {
      auto component = type->parseFunction()(entry.toObject(), parseProperties);
      components.append(component);
    }

    components_[type->name()] = components;
  }
}

Extension::~Extension() {}

void Extension::initialize() {
  for (const auto& list : components_)
    for (const auto& component : list) this->setContainer(component);
}

QProcess* Extension::start(const QString& action, const QStringList& arguments,
                           QProcess* process,
                           const QSharedPointer<QString>& scriptOutput) {
  QStringList args2;
  args2 << "--voxie-action=" + action;
  args2.append(arguments);

  return extensionLauncher->startScript(scriptFilename(), nullptr, args2,
                                        process, scriptOutput);
}

void Extension::startOperation(
    const QSharedPointer<vx::ExternalOperation>& exOp,
    const QStringList& arguments) {
  auto initialRef = createQSharedPointer<QSharedPointer<ExternalOperation>>();
  *initialRef = exOp;
  exOp->initialReference = initialRef;

  auto op = exOp->operation();

  QStringList args;
  args << "--voxie-operation=" + exOp->getPath().path();
  args << arguments;
  auto scriptOutput = op->scriptOutput();
  auto process = start(exOp->action(), args, new QProcess(), scriptOutput);
  if (!process) {
    // TODO: Return an error from start() / startScript()?
    // TODO: Check whether this should keep a reference to initialRef
    enqueueOnThread(op.data(), [initialRef, op] {
      op->finish(createQSharedPointer<vx::io::Operation::ResultError>(
          createQSharedPointer<Exception>("de.uni_stuttgart.Voxie.Error",
                                          "Failed to start process")));
    });
    return;
  }
  op->process = process;

  // Note: This lambda also will keep alive initialRef (and therefore exOp until
  // exOp is claimed by the extension or until the process quits)
  connect(
      process, &QObject::destroyed, this, [this, initialRef, scriptOutput]() {
        auto exOpValue = *initialRef;
        initialRef->reset();
        if (exOpValue) {  // exit without claiming the operation
          QString scriptOutputString = *scriptOutput;
          if (scriptOutputString != "")
            scriptOutputString = "\n\nScript output:\n" + scriptOutputString;

          // TODO: Should this contain script output?
          exOpValue->operation()->finish(
              createQSharedPointer<vx::io::Operation::ResultError>(
                  createQSharedPointer<Exception>(
                      "de.uni_stuttgart.Voxie.ExtensionErrorNoClaim",
                      "Extension failed to claim the operation" +
                          scriptOutputString)));
        }
      });
}

// TODO: Also support this for non-operation extension functions?
void Extension::startOperationDebug(
    const QSharedPointer<vx::ExternalOperation>& exOp,
    const QStringList& arguments) {
  auto op = exOp->operation();

  auto service = dbusService;
  if (!service) {
    enqueueOnThread(op.data(), [op] {
      op->finish(createQSharedPointer<vx::io::Operation::ResultError>(
          createQSharedPointer<Exception>(
              "de.uni_stuttgart.Voxie.DBusNotAvailable",
              "Cannot run external script because DBus is not available")));
    });
    return;
  }

  auto initialRef = createQSharedPointer<QSharedPointer<ExternalOperation>>();
  *initialRef = exOp;
  exOp->initialReference = initialRef;

  // TODO: Some merge command line building code with start() / startScript()?

  QStringList args;
  args << "--voxie-action=" + exOp->action();
  args << "--voxie-operation=" + exOp->getPath().path();
  args << arguments;
  service->addArgumentsTo(args);

  auto dialog = extensionLauncher->startDebug(
      this->scriptFilename(), args, this,
      [this, initialRef, arguments]() {
        auto exOpValue = *initialRef;
        if (!exOpValue)
          // Operation has already been claimed
          return;
        initialRef->reset();
        this->startOperation(exOpValue, arguments);
      },
      [initialRef]() {
        auto exOpValue = *initialRef;
        if (!exOpValue)
          // Operation has already been claimed
          return;
        initialRef->reset();
        if (!exOpValue->operation()->isFinished())
          exOpValue->operation()->finish(
              createQSharedPointer<vx::io::Operation::ResultError>(
                  createQSharedPointer<Exception>(
                      "de.uni_stuttgart.Voxie.OperationCancelled",
                      "Execution was cancelled")));
      });

  connect(this, &QObject::destroyed, dialog, &QObject::deleteLater);
  connect(exOp.data(), &ExternalOperation::claimed, dialog,
          &QObject::deleteLater);
  op->onFinished(dialog, [dialog](const auto&) { dialog->deleteLater(); });
}

// TODO: List of file endings where "Edit Source" is displayed?
bool Extension::offerShowSource() const {
  return scriptFilename().endsWith(".py");
}

QList<QSharedPointer<vx::Component>> Extension::listComponents(
    const QSharedPointer<ComponentType>& componentType) {
  if (!components_.contains(componentType->name()))
    throw Exception("de.uni_stuttgart.Voxie.InvalidComponentType",
                    "Unknown component type in extension");

  return components_[componentType->name()];
}

QSharedPointer<vx::Component> Extension::getComponent(
    const QSharedPointer<ComponentType>& componentType, const QString& name,
    bool allowCompatibilityNames) {
  if (!components_.contains(componentType->name()))
    throw Exception("de.uni_stuttgart.Voxie.InvalidComponentType",
                    "Unknown component type in extension");

  QSharedPointer<Component> result;

  for (const auto& component : components_[componentType->name()]) {
    bool found = component->name() == name;
    if (allowCompatibilityNames) {
      for (const auto& n : component->compatibilityNames()) {
        if (n == name) found = true;
      }
    }
    if (!found) continue;
    // TODO: Should this check be done when creating the extension (there
    // probably should not be any ambiguous names inside one extension)
    if (result)
      throw Exception("de.uni_stuttgart.Voxie.AmbiguousComponentName",
                      "Component name '" + name + "' for type '" +
                          componentType->name() + "' is ambiguous");
    result = component;
  }
  if (!result)
    throw Exception("de.uni_stuttgart.Voxie.ComponentNotFound",
                    "Could not find component '" + name + "' with type '" +
                        componentType->name() + "'");

  return result;
}
