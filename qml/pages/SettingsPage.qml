import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Settings")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                text: apiKeyManager.hasDefaultKey
                    ? qsTr("A shared default key is built in, but it's rate-limited across everyone using this build. Add your own free key from the NS API portal for reliable access.")
                    : qsTr("This build has no default key. Register a free key at the NS API portal (apiportal.ns.nl) and paste it below.")
            }

            TextField {
                id: keyField
                width: parent.width
                label: qsTr("NS API key")
                placeholderText: qsTr("Subscription-Key")
                text: apiKeyManager.userKey
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: focus = false
                onTextChanged: apiKeyManager.userKey = text
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: apiKeyManager.hasUsableKey ? Theme.secondaryColor : Theme.errorColor
                text: apiKeyManager.hasUsableKey
                    ? qsTr("Requests remaining in this 5-minute window: %1").arg(requestBudget.remaining)
                    : qsTr("No usable key yet - trip planning is disabled until one is set.")
            }
        }
    }
}
