import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material 2.15
import Qt.labs.platform 1.1

import CryptoUtils

ColumnLayout{
    Label{
        text:"encryption file"
        font.pointSize: 15
    }
    ColumnLayout{
        Label{
            text: "select public key"
            font.pointSize: 10
        }
        RowLayout{
            CustomButton{
                Layout.preferredWidth: parent.width / 5
                Layout.fillHeight: true
                text: "select path"
                onClicked: fileDialogPublicKeyEncrypt.open()
            }
            TextField{
                id:publicKeyPathEncrypt
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "select public key"
            }
            FileDialog {
                id: fileDialogPublicKeyEncrypt
                title: "select files"
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                nameFilters: ["key (*.pem)", "All files (*)"]

                onAccepted: {
                    publicKeyPathEncrypt.text = String(fileDialogPublicKeyEncrypt.file).replace("file:///", "")
                }
            }
        }
    }
    ColumnLayout{
        Label{
            text: "select file"
            font.pointSize: 10
        }
        RowLayout{
            CustomButton{
                Layout.preferredWidth: parent.width / 5
                Layout.fillHeight: true
                text: "select file"
                onClicked: fileDialogFileEncrypt.open()
            }
            TextField{
                id:fileEncrypt
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "select file"
            }
            FileDialog {
                id: fileDialogFileEncrypt
                title: "select files"
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                nameFilters: ["text files (*.txt)", "All files (*)"]

                onAccepted: {
                    fileEncrypt.text = String(fileDialogFileEncrypt.file).replace("file:///", "")
                }
            }
        }
    }
    CustomButton {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: "encrypt file"
        onClicked: {
            if (publicKeyPathEncrypt.text === "" || fileEncrypt.text === "") {
                statusLabel.text = "Please select all required files"
                return
            }

            var encryptedFile = fileEncrypt.text+".enc";

            if (cryptoUtils.encryptFile(fileEncrypt.text, encryptedFile, publicKeyPathEncrypt.text)) {
                statusLabel.text = "File encrypted successfully"
            } else {
                statusLabel.text = "Failed to encrypt file"
            }
        }
    }
}
