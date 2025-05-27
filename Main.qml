import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material 2.15
import QtQuick.Dialogs 6.0
import QtQuick.Window 2.15

import WorkWithEncryption 1.0

ApplicationWindow {
    width: 600
    height: 700

    minimumWidth: 500
    minimumHeight: 600

    visible: true
    title: qsTr("File Encryption Tool")

    Material.theme: Material.Dark
    Material.accent: Material.LightBlue
    Material.primary: Material.Grey
    Material.foreground: Material.White
    Material.background: Material.color(Material.Grey, Material.Shade900)
    Material.elevation: 6

    Rectangle {
        anchors.fill: parent
        color: Material.background
    }

    ColumnLayout{
        width: parent.width * 0.95
        height: parent.height * 0.95
        anchors.centerIn: parent
        CreateKeysForm {
            id:createKeys
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height / 3
        }
        EncryptFileForm {
            id:encryptFile
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height / 3
        }
        DecryptFileForm {
            id:decryptFile
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height / 3
        }
        Label{
            id: statusLabel;
            text:""
        }

    }
}
