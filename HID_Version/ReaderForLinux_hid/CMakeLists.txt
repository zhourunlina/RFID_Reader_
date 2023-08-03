# CMakeList.txt: ReaderForLinux 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


# 将源代码添加到此项目的可执行文件。
add_executable (ReaderForLinux  "ReaderForLinux.cpp" "ReaderForLinux.h" "CSerialPort.h" "CSerialPort.cpp" "hidapi.h" "HidrawManager.h" "HidrawManager.cpp" "ProtocolPacket_RFID.h" "ProtocolPacket_RFID.cpp" "ProtocolPacket_TMCM.h" "ProtocolPacket_TMCM.cpp" "ProtocolPacket_XMT.h" "ProtocolPacket_XMT.cpp" "Tools.h" "Tools.cpp" "ConvertData.h""ConvertData.cpp" "DES.h" "DES.cpp")
# 链接动态链接库.so文件
target_link_libraries(ReaderForLinux -lhidapi-hidraw -ludev)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ReaderForLinux PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
