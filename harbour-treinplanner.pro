# The name of your app
TARGET = harbour-treinplanner

CONFIG += sailfishapp c++11

QT += network

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# OpenRepos/sideload builds define openrepos_build and provide NS_API_DEFAULT_KEY
# in the build environment to bake in a default API key. Harbour builds must
# never set this - see rpm/harbour-treinplanner.yaml vs rpm/treinplanner-openrepos.yaml.
openrepos_build {
    DEFINES += "TREINPLANNER_DEFAULT_API_KEY=\\\"$$(NS_API_DEFAULT_KEY)\\\""
}

SOURCES += \
    src/main.cpp \
    src/nsapi/NsApiClient.cpp \
    src/app/TripPlannerController.cpp \
    src/models/StationListModel.cpp \
    src/models/TripResultsModel.cpp \
    src/station/StationRepository.cpp \
    src/station/StationBootstrap.cpp \
    src/network/RequestBudgetTracker.cpp \
    src/network/NetworkStateMonitor.cpp \
    src/cache/TripResultCache.cpp \
    src/settings/ApiKeyManager.cpp \
    src/util/NsDateTime.cpp

HEADERS += \
    src/nsapi/NsApiClient.h \
    src/app/TripPlannerController.h \
    src/models/StationListModel.h \
    src/models/TripResultsModel.h \
    src/station/StationRepository.h \
    src/station/StationBootstrap.h \
    src/network/RequestBudgetTracker.h \
    src/network/NetworkStateMonitor.h \
    src/cache/TripResultCache.h \
    src/settings/ApiKeyManager.h \
    src/util/NsDateTime.h

DISTFILES += \
    qml/harbour-treinplanner.qml \
    qml/cover/*.qml \
    qml/pages/*.qml \
    qml/components/*.qml \
    rpm/harbour-treinplanner.yaml \
    rpm/treinplanner-openrepos.yaml \
    harbour-treinplanner.desktop \
    data/stations_nl.json

data.files = data/stations_nl.json
data.path = /usr/share/$${TARGET}/data
INSTALLS += data

TRANSLATIONS += translations/harbour-treinplanner-nl.ts
