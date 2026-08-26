import QtQuick 2.0
import Sailfish.Silica 1.0

ValueButton {
    id: root

    property string title: ""
    property var station: null
    property string countryCode: "NL"

    signal stationChosen(var station)

    label: title
    value: station ? station.nameLong : qsTr("Select station")

    onClicked: {
        var page = pageStack.push(Qt.resolvedUrl("../pages/StationSearchPage.qml"), {countryCode: countryCode})
        page.stationChosen.connect(function(selected) {
            root.station = selected
            root.stationChosen(selected)
        })
    }
}
