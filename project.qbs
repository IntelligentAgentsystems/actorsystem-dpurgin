import qbs

QtGuiApplication {
    cpp.cxxLanguageVersion: "c++17"

    cpp.defines: [
        // The following define makes your compiler emit warnings if you use
        // any Qt feature that has been marked deprecated (the exact warnings
        // depend on your compiler). Please consult the documentation of the
        // deprecated API in order to know how to port your code away from it.
        "QT_DEPRECATED_WARNINGS",

        // You can also make your code fail to compile if it uses deprecated APIs.
        // In order to do so, uncomment the following line.
        // You can also select to disable deprecated APIs only up to a certain version of Qt.
        //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
        "CAF_ENABLE_RUNTIME_CHECKS",

        "QT_NO_KEYWORDS"
    ]

    consoleApplication: true
    files: [
        "agentsystemconnector.cpp",
        "agentsystemconnector.h",
        "main.cpp",
        "agentsystem.cpp",
        "agentsystem.h",
        "model/Laptop.cpp",
        "model/Laptop.h",
        "model/Plotter.cpp",
        "model/Plotter.h",
        "model/Turntable.cpp",
        "model/Turntable.h",
        "project.qrc",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }

    Depends {
        name: "Qt.qml"
    }
    Depends {
        name: "Qt.network"
    }

    property string cafPath: "D:/MyDocs/Documents/Studium/9_AGS/caf"
    property string qmlImportPath: "D:/MyDocs/Documents/Studium/9_AGS/project/qml"

    cpp.includePaths: [cafPath + "/include"]
    cpp.libraryPaths: [cafPath + "/lib/" + qbs.buildVariant]
    cpp.staticLibraries: [
        "Iphlpapi.lib",
        "Ws2_32.lib",
        "caf_core_static.lib",
        "caf_io_static.lib"
    ]
}
