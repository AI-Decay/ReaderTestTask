#include <QCoreApplication>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include "SimpleTimer.hpp"
#include "..\\toml11-3.4.0\\toml.hpp"

auto FileSize(std::wstring path) {
    std::ifstream file( path, std::ios::binary | std::ios::ate);
    return file.tellg();
}

bool Read(std::wstring path, size_t speed = 1) {
    SimpleTimer timer;
    size_t pos = 0;
    size_t bufferSize = 1024;
    size_t readSpeed = speed * bufferSize;
    std::ifstream file{ path };
    if(!file)
        return false;

    size_t size = FileSize(path);
    size_t onePersent = size/100;
    size_t progress = 0;

    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    std::cout << "Size : " << size << std::endl;
    if(size <= readSpeed)
        readSpeed = size;
    timer.Start();
    std::cout << std::fixed << "Read...";
    while (pos < size)
    {
        for(size_t cur = 0; (pos < size) && (cur < readSpeed); cur += bufferSize)
        {
            if(pos + bufferSize > size)
               bufferSize = size - pos;
             file.read(buffer.get(), bufferSize);
             pos += bufferSize;
             file.seekg(pos);

             if(pos > progress)
             {
                 std::cout << '|';
                 progress += onePersent;
             }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 - timer.Duration()));
        timer.ResetTime();
    }

     std::cout << std::endl << "File has been read" << std::endl;

    return true;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::wstring directory{};
    std::cout << "Enter the directory:" << std::endl;
    std::wcin >> directory;
    while(!std::filesystem::exists(directory))
    {
        std::cout << "Directory does not exist" << std::endl;
        std::cout << "Enter the directory:" << std::endl;
        std::wcin >> directory;
    }
    std::vector<std::wstring> directoryData;
    std::wstring path{};
    std::cout << "List of files in this directory:" << std::endl;
    for (const auto & entry : std::filesystem::directory_iterator(directory))
    {
            path = entry.path();
            directoryData.push_back(path);
            if(!std::filesystem::is_directory(path))
               std::wcout << path << std::endl;
    }

    std::wstring fileName{};
    std::cout << "Enter the name of file:" << std::endl;
    std::wcin >> fileName;
    while(!std::filesystem::exists(directory + fileName) || std::filesystem::is_directory(directory + fileName))
    {
        std::cout << "File does not exist" << std::endl;
        std::cout << "Enter the name of file:" << std::endl;
        std::wcin >> fileName;
    }

    int rSpeed;
    std::cout << "Enter the name reading speed" << std::endl;
    std::cin >> rSpeed;
    while(rSpeed < 0 || std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input, speed must be number and more than 0" << std::endl;
        std::cout << "Enter the name reading speed" << std::endl;
        std::cin >> rSpeed;
    }

    std::thread reading(Read, directory + fileName, rSpeed);

    toml::value data{};
    std::wofstream out("info.toml");
    if (out.is_open())
    {
        for(const std::wstring& path : directoryData)
        {
            if(!std::filesystem::is_directory(path))
               data = { { "file in this directory", path} };
            else
               data = { { "subdirectory in this directory", path} };
           out << std::setprecision(30) << data << std::endl;
        }
    }
    out.close();

    reading.join();

    return a.exec();
}
