import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    property string countryCode: "NL"

    signal stationChosen(var station)

    function doSearch() {
        tripPlanner.searchStations(searchField.text, countryCode)
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        header: Column {
            width: page.width

            PageHeader {
                title: qsTr("Select station")
            }

            SearchField {
                id: searchField
                width: parent.width
                placeholderText: qsTr("Station name, then Enter")
                EnterKey.iconSource: "image://theme/icon-m-enter-search"
                EnterKey.onClicked: doSearch()
                Keys.onReturnPressed: doSearch()
            }
        }

        model: stationListModel

        delegate: BackgroundItem {
            id: delegateItem
            width: listView.width
            height: nameColumn.height + 2 * Theme.paddingMedium

            Column {
                id: nameColumn
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                Label {
                    width: parent.width
                    text: nameLong
                    truncationMode: TruncationMode.Fade
                    color: delegateItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
                Label {
                    text: code
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: delegateItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                }
            }

            onClicked: {
                page.stationChosen({
                    code: code,
                    nameLong: nameLong,
                    nameMedium: nameMedium,
                    nameShort: nameShort
                })
                pageStack.pop()
            }
        }

        VerticalScrollDecorator {}

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No stations found")
            hintText: qsTr("Type a station name and press Enter")
        }
    }
}
