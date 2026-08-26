import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    SilicaListView {
        id: listView
        anchors.fill: parent

        header: PageHeader {
            title: qsTr("Trips")
        }

        model: tripResultsModel
        delegate: TripListItem {}

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: tripPlanner.refreshTrip()
            }
        }

        VerticalScrollDecorator {}

        ViewPlaceholder {
            enabled: listView.count === 0 && !tripPlanner.searching
            text: qsTr("No trips found")
        }
    }

    BusyLabel {
        running: tripPlanner.searching
        text: qsTr("Searching…")
    }
}
