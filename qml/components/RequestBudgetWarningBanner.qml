import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: banner

    property bool warning: false

    visible: warning
    height: visible ? label.implicitHeight + 2 * Theme.paddingSmall : 0
    color: Theme.highlightBackgroundColor
    opacity: 0.9

    Label {
        id: label
        anchors.centerIn: parent
        width: parent.width - 2 * Theme.horizontalPageMargin
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        font.pixelSize: Theme.fontSizeExtraSmall
        text: qsTr("Approaching this key's request limit for the next few minutes")
    }
}
