#include <QCoreApplication>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include "SimpleTimer.hpp"

auto FileSize(std::string path) {
    std::ifstream file( path, std::ios::binary | std::ios::ate);
    return file.tellg();
}

bool Read(std::string path, int speed = 1) {
    SimpleTimer timer;
    size_t pos = 0;
    size_t bufferSize = 1024 * 1024;
    size_t readSpeed = speed * bufferSize;
    std::ifstream file{ path };
    size_t size = FileSize(path);
    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    std::cout << "Size" << size << std::endl;
    if(size <= readSpeed)
        readSpeed = size;
    timer.Start();
    while (pos < size)
    {
        for(size_t cur = 0; cur < readSpeed; cur += bufferSize)
        {
            if(pos + bufferSize > size)
                bufferSize = size - pos;
             file.read(buffer.get(), bufferSize);
             pos += bufferSize;
             file.seekg(pos);
             std::cout << std::fixed << "Read..."  << static_cast<double>(pos)/size * 100 << '%' << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 - timer.Duration()));
        timer.ResetTime();
    }


    return true;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::string directory{};
    std::cout << "Enter the directory:" << std::endl;
    std::cin >> directory;
    std::cout << "List of files in this directory:" << std::endl;
    for (const auto & entry : std::filesystem::directory_iterator(directory))
            std::cout << entry.path() << std::endl;

    std::string fileName{};
    std::cout << "Enter the name of file:" << std::endl;
    std::cin >> fileName;

    const auto filepath = directory + fileName;
    Read(filepath, 1000000);

    return a.exec();
}
