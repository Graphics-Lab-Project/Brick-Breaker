// OWNER: Phase 0. Read-only.
#include <QtQuickTest>
#include <QQmlEngine>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(BrickBreakerPlugin)

class Setup : public QObject
{
    Q_OBJECT
public slots:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        engine->addImportPath(QStringLiteral("qrc:/qt/qml"));
    }
};

QUICK_TEST_MAIN_WITH_SETUP(bb_qmltest, Setup)

#include "qml_runner.moc"
