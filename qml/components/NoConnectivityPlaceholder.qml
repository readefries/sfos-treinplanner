import QtQuick 2.0
import Sailfish.Silica 1.0

Column {
    id: root

    height: visible ? implicitHeight : 0
    spacing: Theme.paddingSmall

    Label {
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        color: Theme.secondaryColor
        text: qsTr("No network connection")
    }
    Label {
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        text: qsTr("Station search still works offline; trip planning needs a connection")
    }
}
