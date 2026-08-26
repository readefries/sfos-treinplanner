import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    id: item
    contentHeight: column.height + 2 * Theme.paddingMedium

    Column {
        id: column
        x: Theme.horizontalPageMargin
        y: Theme.paddingMedium
        width: parent.width - 2 * Theme.horizontalPageMargin
        spacing: Theme.paddingSmall

        Row {
            width: parent.width
            spacing: Theme.paddingMedium

            Label {
                text: Qt.formatTime(plannedDeparture, "hh:mm") + " – " + Qt.formatTime(plannedArrival, "hh:mm")
                font.pixelSize: Theme.fontSizeMedium
                color: cancelled ? Theme.errorColor : (item.highlighted ? Theme.highlightColor : Theme.primaryColor)
            }
            DelayBadge { minutes: departureDelayMinutes }
            DelayBadge { minutes: arrivalDelayMinutes }
        }

        Label {
            text: cancelled
                ? qsTr("Cancelled")
                : qsTr("%n transfer(s)", "", transfers)
            font.pixelSize: Theme.fontSizeExtraSmall
            color: cancelled ? Theme.errorColor : Theme.secondaryColor
        }

        Repeater {
            model: legs
            delegate: LegRow {
                width: column.width
                leg: modelData
            }
        }
    }
}
