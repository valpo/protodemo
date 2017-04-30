#ifndef PROTODEMO_H
#define PROTODEMO_H

#include <vector>
#include <QMainWindow>
#include "ui_protodemo.h"
#include "demo.pb.h"

class ProtoDemo : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
public:
    explicit ProtoDemo(QWidget *parent = 0);
signals:
    void sendBuffer(const QByteArray&);
    void gotMsg(const google::protobuf::Message&);
public slots:
    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void receiveBuffer(const QByteArray&);
    void handleReceivedMsg(const google::protobuf::Message& msg);
private: //  methods
    void initProtobuf();
    void sendMsg(const google::protobuf::Message& msg);
    std::string msgName(const google::protobuf::Message &msg) const;
private:
    using MessageNames = std::vector<std::string>;
    MessageNames messageNames;
};

#endif // PROTODEMO_H
