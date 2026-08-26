import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    id: cover

    Label {
        anchors.centerIn: parent
        width: parent.width - 2 * Theme.paddingLarge
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        text: tripResultsModel.count > 0
            ? qsTr("Next: %1").arg(Qt.formatTime(tripResultsModel.get(0).plannedDeparture, "hh:mm"))
            : qsTr("Treinplanner")
    }

    CoverActionList {
        CoverAction {
            iconSource: "image://theme/icon-cover-refresh"
            onTriggered: tripPlanner.refreshTrip()
        }
    }
}
