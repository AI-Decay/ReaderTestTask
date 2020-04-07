#include <QCoreApplication>
#include <QDir>
#include <QDataStream>
#include <QFile>
#include <QVector>
#include <QElapsedTimer>
#include <QThread>
#include <iostream>

bool Read(QString path, int speed = 1) {
    QFile file { path };
    QElapsedTimer timer;

    const unsigned int readSpeed = speed * 1024;
    double size = file.size();
        if ( !file.open(QIODevice::ReadOnly) )
        {
            std::cout << file.errorString().toUtf8().constData();
            return false;
        }

        const auto memory = file.map( 0, size );
        if ( memory == nullptr )
        {
            std::cout << "Mapping fialed!"  << std::endl;;
            return false;
        }

        std::cout << file.size() << std::endl;

        size_t pos = 0;

        timer.start();
        while(file.read(readSpeed).size() > 0)
        {
            std::cout << "Read..."  << pos/size * 100 << '%' << std::endl;
            pos += readSpeed;
            file.seek(pos);
            QThread::usleep(1000000 - timer.elapsed() * 1000);
            timer.restart();
        }

        if( pos != size )
        {
           std::cout << "Read..."  << 100 << '%' << std::endl;
        }

        if ( !file.unmap( memory ) )
        {
            std::cout << "Unmapping failed!"  << std::endl;;
            return false;
        }
    return true;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::string input{};
    std::cout << "Enter the directory:" << std::endl;
    std::cin >> input;
    QDir directory(QString::fromStdString(input));
    QStringList files = directory.entryList(QStringList(),QDir::Files);

    std::cout << "List of files in this directory:" << std::endl;
    foreach(QString filename, files) {
    std::cout << filename.toUtf8().constData() << std::endl;
     }


    std::cout << "Enter the name of file:" << std::endl;
    std::cin >> input;

    const auto filepath = "E:\\" + input;
    Read(QString::fromStdString(filepath), 100000);

    return a.exec();
}
