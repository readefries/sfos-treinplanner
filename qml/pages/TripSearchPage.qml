import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    property var fromStation: null
    property var toStation: null
    property string mode: "now"
    property date explicitDateTime: new Date()

    function canSearch() {
        return fromStation !== null && toStation !== null
    }

    function submit() {
        if (!canSearch())
            return
        tripPlanner.planTrip(fromStation.code, toStation.code, mode, explicitDateTime)
    }

    Connections {
        target: tripPlanner
        onTripResultsReady: pageStack.push(Qt.resolvedUrl("TripResultsPage.qml"))
        onNoUsableApiKeyError: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
    }

    Component {
        id: datePickerDialogComponent
        DatePickerDialog {
            date: explicitDateTime
            onAccepted: {
                var pickedDate = date
                var timeDialog = pageStack.push(timePickerDialogComponent, {
                    hour: explicitDateTime.getHours(),
                    minute: explicitDateTime.getMinutes()
                })
                timeDialog.pickedDate = pickedDate
            }
        }
    }

    Component {
        id: timePickerDialogComponent
        TimePickerDialog {
            property date pickedDate: explicitDateTime
            onAccepted: {
                explicitDateTime = new Date(pickedDate.getFullYear(), pickedDate.getMonth(), pickedDate.getDate(), hour, minute)
            }
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Swap stations")
                enabled: fromStation !== null || toStation !== null
                onClicked: {
                    var tmp = fromStation
                    fromStation = toStation
                    toStation = tmp
                }
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
        }

        Column {
            id: column
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Treinplanner")
            }

            RequestBudgetWarningBanner {
                width: parent.width
                warning: requestBudget.nearLimit
            }

            StationValueButton {
                width: parent.width
                title: qsTr("From")
                station: page.fromStation
                onStationChosen: page.fromStation = station
            }

            StationValueButton {
                width: parent.width
                title: qsTr("To")
                station: page.toStation
                onStationChosen: page.toStation = station
            }

            ComboBox {
                width: parent.width
                label: qsTr("When")
                currentIndex: mode === "now" ? 0 : (mode === "departAt" ? 1 : 2)
                menu: ContextMenu {
                    MenuItem { text: qsTr("Depart now") }
                    MenuItem { text: qsTr("Depart at…") }
                    MenuItem { text: qsTr("Arrive by…") }
                }
                onCurrentIndexChanged: {
                    mode = currentIndex === 0 ? "now" : (currentIndex === 1 ? "departAt" : "arriveBy")
                }
            }

            ValueButton {
                width: parent.width
                visible: mode !== "now"
                label: qsTr("Date & time")
                value: Qt.formatDateTime(explicitDateTime, "d MMM, hh:mm")
                onClicked: pageStack.push(datePickerDialogComponent)
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Plan trip")
                enabled: canSearch() && !tripPlanner.searching
                onClicked: submit()
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                visible: tripPlanner.lastErrorMessage.length > 0
                text: tripPlanner.lastErrorMessage
                color: Theme.errorColor
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
            }

            NoConnectivityPlaceholder {
                width: parent.width
                visible: !tripPlanner.isOnline
            }
        }

        BusyLabel {
            running: tripPlanner.searching
            text: qsTr("Searching…")
        }
    }
}
