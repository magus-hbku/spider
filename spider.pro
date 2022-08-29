
HEADERS = $$PWD/glwindow.h \
          $$PWD/subdivision_sphere.h \
          $$PWD/textured_sphere_map.h \
          $$PWD/view_animation.h

SOURCES = $$PWD/glwindow.cpp \
          $$PWD/main.cpp \
          $$PWD/subdivision_sphere.cpp \
          $$PWD/textured_sphere_map.cpp \
          $$PWD/view_animation.cpp

RESOURCES += spider.qrc

target.path = $$PWD/bin
INSTALLS += target

windows {
   CONFIG += glew
}

android {
    QT += sensors
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    assets.files = spider.frag spider.vert scene_test.png
    assets.path = /assets/data
    INSTALLS += assets

    DISTFILES += \
        android/AndroidManifest.xml \
        android/build.gradle \
        android/res/values/libs.xml
}

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    spider.frag \
    spider.vert \
    scene_test.png

glew {
    INCLUDEPATH += $$PWD/external/include
    LIBS += -L$$PWD/external/lib/glew32.lib  opengl32.lib
    message("Configured for glew!!")
}
