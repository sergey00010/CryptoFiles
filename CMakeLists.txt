cmake_minimum_required(VERSION 3.16)

project(ForcksCryptoFiles VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#set your openssl directory
if(MSVC)
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64/")
    # set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
elseif(MINGW)
    set(OPENSSL_ROOT_DIR "C:/msys64/mingw64")
    # set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
elseif(ANDROID)
    set(OPENSSL_ROOT_DIR "D:/Downloads/build/build/openssl/arm64-v8a")

    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.a")
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a")

    message("OPENSSL_ROOT_DIR = ${OPENSSL_ROOT_DIR}")
    message("OPENSSL_INCLUDE_DIR = ${OPENSSL_INCLUDE_DIR}")
else()
    message(WARNING "Unknown compiler, using default OpenSSL path")
    set(OPENSSL_ROOT_DIR "C:/OpenSSL")
    # set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
endif()


set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.5)



if (WIN32)
    set(RC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
    set_source_files_properties(${RC_FILE} PROPERTIES LANGUAGE RC)
    qt_add_executable(appForcksCryptoFiles
        main.cpp
        ${RC_FILE}
    )
else()
    qt_add_executable(appForcksCryptoFiles
        main.cpp
    )
endif()

qt_add_qml_module(appForcksCryptoFiles
    URI ForcksCryptoFiles
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES cryptoutils.h cryptoutils.cpp
        QML_FILES CustomButton.qml
        QML_FILES CreateKeysForm.qml
        QML_FILES EncryptFileForm.qml
        QML_FILES DecryptFileForm.qml
        SOURCES workwithencryption.h workwithencryption.cpp
        QML_FILES
        RESOURCES images.qrc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appForcksCryptoFiles PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appForcksCryptoFiles
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appForcksCryptoFiles
    PRIVATE Qt6::Quick
        OpenSSL::SSL
        OpenSSL::Crypto
)

include(GNUInstallDirs)
install(TARGETS appForcksCryptoFiles
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

