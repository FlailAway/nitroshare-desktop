/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QDir>
#include <QLibrary>

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/plugin.h>
#include <nitroshare/pluginregistry.h>

#include "pluginregistry_p.h"

const QString LoggerTag = "pluginregistry";

PluginRegistryPrivate::PluginRegistryPrivate(PluginRegistry *parent, Application *application)
    : QObject(parent),
      q(parent),
      application(application)
{
}

PluginRegistryPrivate::~PluginRegistryPrivate()
{
    q->unloadAll();
}

bool PluginRegistryPrivate::isPluginInitialized(const QString &name) const
{
    if (name == "ui") {
        return application->isUiEnabled();
    } else {
        return static_cast<bool>(q->pluginByName(name));
    }
}

void PluginRegistryPrivate::unloadPlugin(int index)
{
    // Remove the plugin from the list and grab its name
    Plugin *plugin = plugins.takeAt(index);
    QString pluginName = plugin->name();

    // Log that a plugin is being unloaded
    application->logger()->log(Logger::Debug, LoggerTag, QString("unloading %1").arg(pluginName));

    // Destroy the plugin and indicate that it has been unloaded
    delete plugin;
    emit q->pluginUnloaded(pluginName);
}

PluginRegistry::PluginRegistry(Application *application, QObject *parent)
    : QObject(parent),
      d(new PluginRegistryPrivate(this, application))
{
}

QList<Plugin*> PluginRegistry::plugins() const
{
    return d->plugins;
}

Plugin *PluginRegistry::pluginByName(const QString &name) const
{
    foreach (Plugin *plugin, d->plugins) {
        if (plugin->name() == name) {
            return plugin;
        }
    }

    return nullptr;
}

void PluginRegistry::loadPluginsFromDirectories(const QStringList &directories)
{
    d->application->logger()->log(
        Logger::Info,
        LoggerTag,
        QString("loading plugins from %1").arg(directories.join(", "))
    );

    // Step 1: build a list of plugins that could be loaded
    QList<Plugin*> uninitializedPlugins;
    foreach (QString directory, directories) {
        QDir dir(directory);
        foreach (QString filename, dir.entryList(QDir::Files)) {
            filename = dir.absoluteFilePath(filename);
            if (!QLibrary::isLibrary(filename)) {
                continue;
            }
            Plugin *plugin = new Plugin(d->application, filename);
            if (!plugin->load()) {
                delete plugin;
                continue;
            }
            uninitializedPlugins.append(plugin);
        }
    }

    // Step 2: go through the list, initializing plugins with their
    // dependencies met until an iteration with no new plugins
    int numPluginsInitialized = 0;
    while (true) {
        int numPluginsInitializedPrev = numPluginsInitialized;
        foreach (Plugin *plugin, uninitializedPlugins) {
            bool missingDependencies = false;
            foreach (QString dependency, plugin->dependencies()) {
                if (!d->isPluginInitialized(dependency)) {
                    missingDependencies = true;
                    break;
                }
            }
            if (!missingDependencies) {
                d->application->logger()->log(
                    Logger::Debug,
                    LoggerTag,
                    QString("initializing %1").arg(plugin->name())
                );
                plugin->initialize();
                uninitializedPlugins.removeOne(plugin);
                d->plugins.append(plugin);
                emit pluginLoaded(plugin->name());
                ++numPluginsInitialized;
            }
        }
        if (numPluginsInitialized == numPluginsInitializedPrev) {
            break;
        }
    }

    // Step 3: show all of the plugins that could not be initialized
    foreach (Plugin *plugin, uninitializedPlugins) {
        QStringList missingDependencies;
        foreach (QString dependency, plugin->dependencies()) {
            if (!d->isPluginInitialized(dependency)) {
                missingDependencies.append(dependency);
            }
        }
        d->application->logger()->log(
            Logger::Error,
            LoggerTag,
            QString("cannot load %1 - missing dependencies: %2")
                .arg(plugin->name())
                .arg(missingDependencies.join(", "))
        );
    }

    // Step 4: unload any plugins with unmet dependencies
    qDeleteAll(uninitializedPlugins);

    d->application->logger()->log(
        Logger::Info,
        LoggerTag,
        QString("loaded %1 plugin(s)").arg(numPluginsInitialized)
    );
}

void PluginRegistry::unloadPlugin(const QString &name)
{
    // Step 1: confirm the plugin exists
    Plugin *rootPlugin = pluginByName(name);
    if (!rootPlugin) {
        return;
    }

    // Step 2: build a list of plugins that must be unloaded
    QList<Plugin*> pluginsToUnload{rootPlugin};
    QList<QString> pluginsToUnloadNames{name};
    for (int i = d->plugins.indexOf(rootPlugin) + 1; i < d->plugins.length(); ++i) {
        Plugin *plugin = d->plugins.at(i);
        foreach (QString dependency, plugin->dependencies()) {
            if (pluginsToUnloadNames.contains(dependency)) {
                pluginsToUnload.append(plugin);
                pluginsToUnloadNames.append(name);
            }
        }
    }

    // Step 3: unload each of the plugins in the list (reverse order)
    for (int i = pluginsToUnload.length() - 1; i >= 0; --i) {
        d->unloadPlugin(d->plugins.indexOf(pluginsToUnload.at(i)));
    }
}

void PluginRegistry::unloadAll()
{
    // Unloading all plugins takes advantage of the order of plugins - working
    // backwards ensures plugins are unloaded before their dependencies
    while (d->plugins.length()) {
        d->unloadPlugin(d->plugins.length() - 1);
    }
}

