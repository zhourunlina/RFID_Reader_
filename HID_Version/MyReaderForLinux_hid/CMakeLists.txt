# CMakeList.txt: MyReaderForLinux 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("MyReaderForLinux")

# 将源代码添加到此项目的可执行文件。

find_package(Threads REQUIRED)
add_executable (MyReaderForLinux "MyReaderForLinux.cpp" "hidapi.h" "HidrawManager.h" "HidrawManager.cpp" "Tools.h" "Tools.cpp" "ConvertData.h" "ConvertData.cpp" "ProtocolPacket_RFID.h" "ProtocolPacket_RFID.cpp"  "DES.h" "DES.cpp")
# 链接动态链接库.so文件
target_link_libraries(${CMAKE_PROJECT_NAME} Threads::Threads -lhidapi-hidraw -ludev)

# TODO: 如有需要，请添加测试并安装目标。
