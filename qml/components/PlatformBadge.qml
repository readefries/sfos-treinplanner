import QtQuick 2.0
import Sailfish.Silica 1.0

Label {
    property string track: ""
    property string plannedTrack: ""

    text: track.length > 0 ? track : "–"
    font.pixelSize: Theme.fontSizeExtraSmall
    color: (plannedTrack.length > 0 && track !== plannedTrack) ? Theme.errorColor : Theme.secondaryColor
}
