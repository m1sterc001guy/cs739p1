#include <iostream>
#include <memory>
#include <string>
#include <stdint.h>

#include <grpc++/grpc++.h>
#include "student.h"

#include "perf.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerWriter;
using grpc::ServerReader;
using perfmeasurements::IntMessage;
using perfmeasurements::IntReply;
using perfmeasurements::DoubleMessage;
using perfmeasurements::DoubleReply;
using perfmeasurements::StringMessage;
using perfmeasurements::StringReply;
using perfmeasurements::StudentMessage;
using perfmeasurements::PerfService;

using namespace std;

class PerfServiceImpl final : public PerfService::Service {
  Status SendInt(ServerContext* context, const IntMessage* request,
                 IntReply* reply) override {
    int integerReply = request->number() * 2;
    reply->set_replynumber(integerReply);
    return Status::OK;
  }

  Status SendDouble(ServerContext* context, const DoubleMessage* request,
                    DoubleReply* reply) override {
    double doubleReply = request->number() * 2;
    reply->set_replynumber(doubleReply);
    return Status::OK;
  }

  Status SendString(ServerContext* context, const StringMessage* request,
                    StringReply* reply) override {
    string stringToReturn("Your message was: " + request->stringmessage());
    reply->set_stringreply(stringToReturn);
    return Status::OK;
  }

  Status GetCollegeAdmission(ServerContext* context, const StudentMessage* request,
                             StringReply* reply) override {
    Student student(request->gpa(), request->num_classes());
    string stringToReturn("NOT ADMITTED");
    if (student.get_gpa() > 3.0) {
      stringToReturn = "ADMITTED";
    }
    reply->set_stringreply(stringToReturn);
    return Status::OK;
  }

  Status ListColleges(ServerContext* context, const StringMessage* request,
                      ServerWriter<StringReply>* writer) override {
    //generate random strings
    uint64_t len = 1000000;
    char* rand_bytes = (char *)malloc(len);
    for (int i = 0; i < 100; i++) {
      gen_random(rand_bytes, len);
      string stringmessage(rand_bytes);
      StringReply reply;
      reply.set_stringreply(stringmessage);
      writer->Write(reply);
    }
    return Status::OK;
  }

  Status SendPreferences(ServerContext* context, ServerReader<StringMessage>* reader,
                         StringReply* stringreply) override {
    StringMessage stringmessage;
    while (reader->Read(&stringmessage)) {
      cout << "Received message of size: " << stringmessage.stringmessage().length() << endl;
    }
    stringreply->set_stringreply("Received all messages");
    return Status::OK;
  }

  void gen_random(char *s, const uint64_t len) {
    static const char alphanum[] = 
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

    for(uint64_t i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
  }
};

void RunServer() {
  string server_address("0.0.0.0:50051");
  PerfServiceImpl service;

  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  builder.RegisterService(&service);

  unique_ptr<Server> server(builder.BuildAndStart());
  cout << "Server listening on " << server_address << endl;
  server->Wait();
}

int main(int argc, char** argv) {
  cout << "Server Running..." << endl;
  RunServer();
  return 0;
}
