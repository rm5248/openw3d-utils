#include "rmmixviewer.h"

#include <QApplication>
#include <iostream>

#include "mixfile.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RMMixViewer w;
    w.show();

    openw3d::MIXFile mx;
    mx.open("/home/rob/Renegade/always.dat");

    std::cout << "Num files: " << mx.filenames().size() << "\n";
    for(const std::string& filename : mx.filenames()){
        std::cout << filename << "\n";
    }
    std::cout.flush();

    std::cout << "\n\n";

    std::optional<std::span<const char>> game_ini =
            mx.get_file("game.ini");

    if(!game_ini){
        std::cout << "Unable to read game.ini";
    }else{
        for(const char c : *game_ini){
            std::cout << c;
        }
    }

    std::cout.flush();

    return a.exec();
}
