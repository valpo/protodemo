#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/compiler/parser.h>

#include <QFile>
#include <QDebug>

#include <algorithm>
#include <iterator>

#include "protodemo.h"
#include "demo.pb.h"

using namespace std;

ProtoDemo::ProtoDemo(QWidget *parent) : QMainWindow(parent)
{
    qDebug() << "ProtoDemo created";
    initProtobuf();
    qDebug() << "protobuf initialized";
    setupUi(this);
    qDebug() << "ui ready";
    // simple simulation of sending/receiving messages as stupid buffers
    connect(this, &ProtoDemo::sendBuffer, this, &ProtoDemo::receiveBuffer);
    // real connection from the message receiver to its clients which handle messages
    // note this have to be a sync connection, as the sender deletes the message after emitting it
    connect(this, &ProtoDemo::gotMsg, this, &ProtoDemo::handleReceivedMsg);
}

void ProtoDemo::on_pushButton_1_clicked()
{
    qDebug() << "pushButton_1";
    protodemo::test1 msg;
    msg.set_value1(lineEdit->text().toStdString());
    sendMsg(msg);
}

void ProtoDemo::on_pushButton_2_clicked()
{
    qDebug() << "pushButton_2";
    protodemo::test2 msg;
    msg.set_value2(lineEdit->text().toStdString());
    sendMsg(msg);
}

void ProtoDemo::on_pushButton_3_clicked()
{
    qDebug() << "pushButton_3";
    protodemo::test3 msg;
    msg.set_value3(lineEdit->text().toStdString());
    sendMsg(msg);
}

void ProtoDemo::initProtobuf()
{
    // load protobuf from resource
    QFile data(":/demo.proto");
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qFatal("cannot read proto resource file");
        return;
    }
    QByteArray protoText = data.readAll();
    qDebug() << "proto text size" << protoText.size();

    // parse the proto file https://cxwangyi.wordpress.com/2010/06/29/google-protocol-buffers-online-parsing-of-proto-file-and-related-data-files/
    using namespace google::protobuf;
    using namespace google::protobuf::io;
    using namespace google::protobuf::compiler;

    FileDescriptorProto file_desc_proto;

    ArrayInputStream proto_input_stream(protoText.data(), protoText.size());
    Tokenizer tokenizer(&proto_input_stream, NULL);
    Parser parser;
    if (!parser.Parse(&tokenizer, &file_desc_proto)) {
        qFatal("Cannot parse .proto file");
    }

    // Here we walk around a bug in protocol buffers that
    // |Parser::Parse| does not set name (.proto filename) in
    // file_desc_proto.
    if (!file_desc_proto.has_name()) {
        file_desc_proto.set_name(data.fileName().toLatin1());
    }

    // qDebug() << file_desc_proto.DebugString().c_str();
    for(int i=0; i < file_desc_proto.message_type_size();++i) {
        const DescriptorProto& dp = file_desc_proto.message_type(i);
        qDebug() << i << dp.name().c_str();
        messageNames.push_back("protodemo."+dp.name());
    }
}

void ProtoDemo::sendMsg(const google::protobuf::Message &msg)
{
    // find the name of the message
    string name = msg.GetTypeName();
    qDebug() << "sending message of type" << name.c_str();
    auto iter = std::find(begin(messageNames),end(messageNames),name);
    Q_ASSERT(iter != messageNames.end());
    auto id = distance(begin(messageNames),iter);
    qDebug() << "msg id" << id;
    QByteArray data((char*)&id, sizeof(id));
    string s = msg.SerializeAsString();
    data.append(QByteArray(s.data()),s.size());
    emit sendBuffer(data);
}

void ProtoDemo::handleReceivedMsg(const google::protobuf::Message &msg)
{
    label_2->setText(QString::fromStdString("got message " + msgName(msg)));
    // here comes the place to handle the messages you are interested in
    if (msgName(msg) == "test1") {
        const protodemo::test1& test1 = dynamic_cast<const protodemo::test1&>(msg);
        lineEdit_2->setText(QString::fromStdString(test1.value1()));
    }
    else if (msgName(msg) == "test2") {
        const protodemo::test2& test2 = dynamic_cast<const protodemo::test2&>(msg);
        lineEdit_2->setText(QString::fromStdString(test2.value2()));
    }
    else if (msgName(msg) == "test3") {
        const protodemo::test3& test3 = dynamic_cast<const protodemo::test3&>(msg);
        lineEdit_2->setText(QString::fromStdString(test3.value3()));
    }
    else
        lineEdit_2->setText("unknown message received");
}

string ProtoDemo::msgName(const google::protobuf::Message& msg) const
{
    std::string name = msg.GetTypeName();
    auto i = name.find_last_of(".");
    if (i == std::string::npos) return name;
    else return name.substr(i+1);

}

void ProtoDemo::receiveBuffer(const QByteArray &data)
{
    MessageNames::difference_type idx = *(data.data());
    qDebug() << "received msg with id" << idx;
    const string& name = messageNames[idx];
    qDebug() << "received message with name" << name.c_str();
    const google::protobuf::Descriptor *desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(name);
    Q_ASSERT(desc);
    const google::protobuf::Message *protoMsg = google::protobuf::MessageFactory::generated_factory()->GetPrototype(desc);
    Q_ASSERT(protoMsg);
    google::protobuf::Message* resultMsg = protoMsg->New();
    Q_ASSERT(resultMsg);
    resultMsg->ParseFromArray(data.data()+sizeof(MessageNames::difference_type), data.size()-sizeof(MessageNames::difference_type));
    emit handleReceivedMsg(*resultMsg);
    delete resultMsg;
}
