import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material 2.15

import QtQuick.Dialogs

import WorkWithEncryption 1.0

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
                nameFilters: ["key (*.pem)", "All files (*)"]

                onAccepted: {
                    privateKeyPathDecrypt.text = selectedFile.toString().replace("file:///", "")
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
                nameFilters: ["All files(*)"]

                onAccepted: {
                    decryptFile.text = selectedFile.toString().replace("file:///", "")
                }
            }
        }
    }
    CustomButton {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: "decrypt file"
        onClicked: {
            fileDialogFileDecryptSave.open()
        }
    }

    FileDialog {
        id: fileDialogFileDecryptSave
        title: "write filename"
        nameFilters: ["All files (*)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            if (privateKeyPathDecrypt.text === "" || decryptFile.text === "") {
                statusLabel.text = "Please select all required files"
                return
            }
            if (workWithEncription.decryptFile(decryptFile.text, decryptFile.text, privateKeyPathDecrypt.text )) {
                statusLabel.text = "File decrypted successfully"
            } else {

                statusLabel.text = "Failed to decrypt file"
            }
        }
    }
}
