import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material 2.15
import Qt.labs.platform 1.1

import CryptoUtils

ColumnLayout{
    function removeFileExtension(path) {
        var lastDot = path.lastIndexOf('.');
        var lastSlash = Math.max(path.lastIndexOf('/'), path.lastIndexOf('\\'));

        if (lastDot > lastSlash) {
            return path.substring(0, lastDot);
        }
        return path;
    }

    Label{
        text:"decrypt file"
        font.pointSize: 15
    }
    ColumnLayout{
        Label{
            text: "select private key"
            font.pointSize: 10
        }
        RowLayout{
            CustomButton{
                Layout.preferredWidth: parent.width / 5
                Layout.fillHeight: true
                text: "select path"
                onClicked: fileDialogPrivateKeyDecrypt.open()
            }
            TextField{
                id:privateKeyPathDecrypt
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "select private key"
            }
            FileDialog {
                id: fileDialogPrivateKeyDecrypt
                title: "select file"
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                nameFilters: ["key (*.pem)", "All files (*)"]

                onAccepted: {
                    privateKeyPathDecrypt.text = String(fileDialogPrivateKeyDecrypt.file).replace("file:///", "")
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
                onClicked: fileDialogDecryptFile.open()
            }
            TextField{
                id:decryptFile
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "select file"
            }
            FileDialog {
                id: fileDialogDecryptFile
                title: "select file"
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                nameFilters: ["text files (*.txt)", "All files(*)"]

                onAccepted: {
                    decryptFile.text = String(fileDialogDecryptFile.file).replace("file:///", "")
                }
            }
        }
    }
    CustomButton {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: "decrypt file"
        onClicked: {
            if (privateKeyPathDecrypt.text === "" || decryptFile.text === "") {
                statusLabel.text = "Please select all required files"
                return
            }

            var withoutEnc = removeFileExtension(decryptFile.text);

            if (cryptoUtils.decryptFile(decryptFile.text, withoutEnc, privateKeyPathDecrypt.text )) {
                statusLabel.text = "File decrypted successfully"
            } else {

                statusLabel.text = "Failed to decrypt file"
            }
        }
    }
}
