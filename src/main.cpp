#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <sailfishapp.h>

#include "app/TripPlannerController.h"
#include "models/StationListModel.h"
#include "models/TripResultsModel.h"
#include "network/NetworkStateMonitor.h"
#include "network/RequestBudgetTracker.h"
#include "settings/ApiKeyManager.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QCoreApplication::setOrganizationName(QStringLiteral("harbour-treinplanner"));
    QCoreApplication::setApplicationName(QStringLiteral("harbour-treinplanner"));

    QScopedPointer<QQuickView> view(SailfishApp::createView());

    TripPlannerController controller;
    view->rootContext()->setContextProperty("tripPlanner", &controller);
    view->rootContext()->setContextProperty("stationListModel", controller.stationListModel());
    view->rootContext()->setContextProperty("tripResultsModel", controller.tripResultsModel());
    view->rootContext()->setContextProperty("apiKeyManager", controller.apiKeyManager());
    view->rootContext()->setContextProperty("requestBudget", controller.requestBudgetTracker());

    view->setSource(SailfishApp::pathTo("qml/harbour-treinplanner.qml"));
    view->show();

    return app->exec();
}
