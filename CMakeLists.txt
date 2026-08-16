cmake_minimum_required(VERSION 3.16)

project(project_OOP VERSION 0.1 LANGUAGES CXX)

# 🌐 Cấu hình UTF-8 toàn cục (Sửa triệt để lỗi font Tiếng Việt & Emoji)
if(MSVC)
    add_compile_options(/utf-8)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔍 Tìm kiếm thư viện Qt (Đã thêm thành phần Charts)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

# 📂 Khai báo danh sách TẤT CẢ các file nguồn và tài nguyên đã chia vào thư mục
set(PROJECT_SOURCES
    src/main.cpp
    src/databasemanager.cpp
    src/databasemanager.h
    src/student.h
    src/student.cpp
    src/user.h
    src/userfactory.h
    src/course.h
    src/person.h
    src/loginwindow.h
    src/loginwindow.cpp
    src/adminwindow.h
    src/adminwindow.cpp
    src/studentwindow.h
    src/studentwindow.cpp
    src/changepassworddialog.h
    src/gradedialog.h
    src/gradedialog.cpp
    src/editstudentdialog.h
    src/editstudentdialog.cpp

    # File tài nguyên
    resources/resources.qrc
)

# 🛠️ Tạo file thực thi dựa trên phiên bản Qt tương ứng
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(project_OOP
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(project_OOP SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(project_OOP
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# 🔗 Liên kết các thư viện Qt Widgets và Qt Charts vào dự án
target_link_libraries(project_OOP PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.project_OOP)
endif()

set_target_properties(project_OOP PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS project_OOP
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(project_OOP)
endif()