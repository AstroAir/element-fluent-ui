#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include "FluentQt/Animation/FluentAnimation.h"

using namespace FluentQt::Animation;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setFixedSize(400, 400);

    auto* button = new QPushButton("Animate Me", &window);
    button->setFixedSize(100, 30);
    button->move(150, 185);

    QObject::connect(button, &QPushButton::clicked, [&]() {
        auto* anim = new FluentAnimation(button, &window);
        anim->scaleTo(1.2, 200)
            .with()
            .rotateTo(10, 200)
            .scaleTo(1.0, 200)
            .with()
            .rotateTo(0, 200)
            .start();
    });

    window.show();
    return app.exec();
}
