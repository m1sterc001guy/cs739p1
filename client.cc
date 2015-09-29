#include <iostream>
#include <memory>
#include <string>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include <grpc++/grpc++.h>

#include "perf.grpc.pb.h"
#include "student.h"
#include <vector>
#include <algorithm>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;
using grpc::ClientWriter;
using perfmeasurements::IntMessage;
using perfmeasurements::IntReply;
using perfmeasurements::DoubleMessage;
using perfmeasurements::DoubleReply;
using perfmeasurements::StringMessage;
using perfmeasurements::StringReply;
using perfmeasurements::StudentMessage;
using perfmeasurements::PerfService;

using namespace std;

#define BILLION 1000000000L

class PerfClient {
  public:
    PerfClient(shared_ptr<Channel> channel)
      : stub_(PerfService::NewStub(channel)) {}

    vector<int> packInts(int trials) {
      vector<int> diffs;
      for (int i = 0; i < trials; i++) {
        struct timespec start, end;
        uint64_t diff;
        IntMessage request;
        clock_gettime(CLOCK_MONOTONIC, &start);
        request.set_number(rand());
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_time_diff(start, end);
        //cout << "Overhead of marshalling an integer: " << diff << " nanoseconds." << endl;
        diffs.push_back(diff);
      } 
      return diffs;
    }

    vector<int> packDoubles(int trials) {
      vector<int> diffs;
      for (int i = 0; i < trials; i++) {
        struct timespec start, end;
        uint64_t diff;
        DoubleMessage request;
        clock_gettime(CLOCK_MONOTONIC, &start);
        request.set_number((double)rand() / RAND_MAX);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_time_diff(start, end);
        //cout << "Overhead of marshalling a double: " << diff << " nanoseconds." << endl;
        diffs.push_back(diff);
      }
      return diffs;
    }

    vector<int> packStrings(int trials, int stringSize, char *rand_bytes) {
      vector<int> diffs;
      //char* rand_bytes = (char *)malloc(stringSize);
      for (int i = 0; i < trials; i++) {
        gen_random(rand_bytes, stringSize);
        string message(rand_bytes);

        struct timespec start, end;
        uint64_t diff;
        StringMessage request;
        clock_gettime(CLOCK_MONOTONIC, &start);
        request.set_stringmessage(message);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_time_diff(start, end);
        //cout << "Overhead of marshalling a string of length " << message.length() << ": " << diff << " nanoseconds." << endl;
        diffs.push_back(diff);
      }
      //free(rand_bytes);
      return diffs;
    }

    int SendInt(const int number) {
      struct timespec start, end;
      uint64_t diff;
      IntMessage request;
      clock_gettime(CLOCK_MONOTONIC, &start);
      request.set_number(number);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      cout << "Overhead of marshalling an integer: " << diff << " nanoseconds." << endl;

      IntReply reply;
      
      ClientContext context;

      clock_gettime(CLOCK_MONOTONIC, &start);
      Status status = stub_->SendInt(&context, request, &reply);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      cout << "RTT for IntMessage: " << diff << " nanoseconds." << endl;

      if (status.ok()) {
        return reply.replynumber();
      } else {
        return 0;
      }
    }

    double SendDouble(const double number) {
      struct timespec start, end;
      uint64_t diff;
      DoubleMessage request;
      clock_gettime(CLOCK_MONOTONIC, &start);
      request.set_number(number);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      cout << "Overhead of marshalling a double: " << diff << " nanoseconds." << endl;

      DoubleReply reply;
      ClientContext context;
      clock_gettime(CLOCK_MONOTONIC, &start);
      Status status = stub_->SendDouble(&context, request, &reply);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      cout << "RTT for DoubleMessage: " << diff << " nanoseconds." << endl;

      if (status.ok()) {
        return reply.replynumber();
      } else {
        return 0.0;
      }
    }

    string SendString(const string &message) {
      struct timespec start, end;
      uint64_t diff;
      StringMessage request;
      clock_gettime(CLOCK_MONOTONIC, &start);
      request.set_stringmessage(message);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      //cout << "Overhead of marshalling a string of length " << message.length() << ": " << diff << " nanoseconds." << endl;

      StringReply reply;
      ClientContext context;
      clock_gettime(CLOCK_MONOTONIC, &start);
      Status status = stub_->SendString(&context, request, &reply);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      cout << "RTT for StringMessage for a string of length " << message.length() << ": " << diff << " nanoseconds." << endl;

      if (status.ok()) {
        return reply.stringreply();
      } else {
        return "RPC returned FAILURE";
      }  
    }

    string SendLargeString(uint64_t len) {
      char* rand_bytes = (char *)malloc(len);
      gen_random(rand_bytes, len);
      string message(rand_bytes);

      struct timespec start, end;
      uint64_t diff;
      StringMessage request;
      clock_gettime(CLOCK_MONOTONIC, &start);
      request.set_stringmessage(message);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      cout << "Overhead of marshalling a string of length " << message.length() << ": " << diff << " nanoseconds." << endl;

      StringReply reply;
      ClientContext context;
      clock_gettime(CLOCK_MONOTONIC, &start);
      Status status = stub_->SendString(&context, request, &reply);
      clock_gettime(CLOCK_MONOTONIC, &end);
      diff = get_time_diff(start, end);
      cout << "RTT for StringMessage for a string of length " << message.length() << ": " << diff << " nanoseconds." << endl;

      if (status.ok()) {
        return reply.stringreply();
      } else {
        return "RPC returned FAILURE";
      }
    }

    string GetCollegeAdmission(Student &student) {
        struct timespec start, end;
        uint64_t diff;
        StudentMessage request;
        clock_gettime(CLOCK_MONOTONIC, &start);
        request.set_gpa(student.get_gpa());
        request.set_num_classes(student.get_number_of_classes());
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_time_diff(start, end);
        cout << "Overhead of marshalling a Student: " << diff << " nanoseconds." << endl;

        StringReply reply;
        ClientContext context;
        clock_gettime(CLOCK_MONOTONIC, &start);
        Status status = stub_->GetCollegeAdmission(&context, request, &reply);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_time_diff(start, end);
        cout << "RTT for StudentMessage: " << diff << " nanoseconds." << endl;

        if (status.ok()) {
          return reply.stringreply();
        } else {
          return "RPC returned FAILURE";
        } 
      }

      void ListColleges() {
        struct timespec start, end;
        uint64_t diff;

        StringMessage stringmessage;
        stringmessage.set_stringmessage("Give me a list of colleges");
        ClientContext context;
        uint64_t totalBytes = 0;

        clock_gettime(CLOCK_MONOTONIC, &start);
        unique_ptr<ClientReader<StringReply>> reader(stub_->ListColleges(&context, stringmessage));
        StringReply reply;
        while (reader->Read(&reply)) {
          //cout << "String returned: " << reply.stringreply() << endl;
          //cout << "Message received. Length: " << reply.stringreply().length() << endl;
          totalBytes += reply.stringreply().length();
        }

        Status status = reader->Finish();
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_time_diff(start, end);
        if (status.ok()) {
          cout << "ListColleges rpc server stream succeeded." << endl;
          double megabytes = (double)totalBytes / 1000000;
          double seconds = (double) diff / 1000000000;
          double megabytesPerSecond = megabytes / seconds;
          //cout << "Total Bytes: " << totalBytes << " Diff: " << diff << " nanoseconds" << endl;
          cout << "Megabytes: " << megabytes << " Seconds: " << seconds << endl;
          cout << "Bandwidth: " << megabytesPerSecond << " Megabytes / Second" << endl;
        } else {
          cout << "ListColleges rpc server stream failed." << endl;
        }
      }

      void SendPreferences(uint64_t len) {
        struct timespec start, end;
        uint64_t diff;
        uint64_t totalBytes = 0;

        ClientContext context;
        StringReply stringreply;

        clock_gettime(CLOCK_MONOTONIC, &start);
        unique_ptr<ClientWriter<StringMessage>> writer(stub_->SendPreferences(&context, &stringreply));
        char* rand_bytes = (char *)malloc(len);
        for (int i = 0; i < 100; i++) {
          gen_random(rand_bytes, len);
          string stringmessage(rand_bytes);
          StringMessage message;
          message.set_stringmessage(stringmessage);
          if (!writer->Write(message)) {
            break; //broken stream
          }
          totalBytes += len;
        }

        writer->WritesDone();
        Status status = writer->Finish();
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_time_diff(start, end);
        if (status.ok()) {
          cout << "SendPreferences Client Streaming rpc finished successfully." << endl;
          double megabytes = (double)totalBytes / 1000000;
          double seconds = (double) diff / 1000000000;
          double megabytesPerSecond = megabytes / seconds;
          //cout << "Total Bytes: " << totalBytes << " Diff: " << diff << " nanoseconds" << endl;
          cout << "Megabytes: " << megabytes << " Seconds: " << seconds << endl;
          cout << "Bandwidth: " << megabytesPerSecond << " Megabytes / Second" << endl;

        } else {
          cout << "SendPreferences Client Streaming rpc failed." << endl;
        }
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

  private:
    unique_ptr<PerfService::Stub> stub_;

    uint64_t get_time_diff(struct timespec &start, struct timespec &end) {
      return BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
    }
};


double getMedian(vector<int> measurements) {
  sort(measurements.begin(), measurements.end());
  if (measurements.size() % 2 == 0) {
    int halfIndex = (measurements.size() / 2);
    return (measurements[halfIndex-1] + measurements[halfIndex]) / (double)2.0;
  }
  return (double)(measurements[measurements.size() / 2]);
}

double getAverage(vector<int> measurements) {
  int sum = 0;
  for (int i = 0; i < measurements.size(); i++) {
    sum += measurements[i];
  }
  return sum / (double) measurements.size();
}

int main(int argc, char** argv) {
  cout << "Client running..." << endl;
  PerfClient perf(grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));

  // Graph 1 code
  /*
  int trials = 1000;
  vector<int> intPackingMeasurements = perf.packInts(trials);
  cout << "Average INT Packing: " << getAverage(intPackingMeasurements) << " nanoseconds." << endl;
  cout << "Median INT Packing: " << getMedian(intPackingMeasurements) << " nanoseconds." << endl;

  vector<int> doublePackingMeasurements = perf.packDoubles(trials);
  cout << "Average DOUBLE Packing: " << getAverage(doublePackingMeasurements) << " nanoseconds." << endl;
  cout << "Median DOUBLE Packing: " << getMedian(doublePackingMeasurements) << " nanoseconds." << endl;

  int num_bytes = 10;
  char* rand_bytes = (char *)malloc(num_bytes);
  vector<int> stringPackingMeasurements = perf.packStrings(trials, num_bytes, rand_bytes); 
  free(rand_bytes);
  cout << "Average STRING Packing: " << getAverage(stringPackingMeasurements) << " nanoseconds." << endl;
  cout << "Median STRING Packing: " << getMedian(stringPackingMeasurements) << " nanoseconds." << endl;
  */

  // Graph 2 code
  /*
  int stringSize = 10;
  char *rand_bytes = (char *)malloc(stringSize);
  for (int i = 0; i < 5; i++) {
    perf.gen_random(rand_bytes, stringSize);
    string message(rand_bytes);
    perf.SendString(message); 
  }
  */

  perf.ListColleges();

  perf.SendPreferences(1000000);

  // This is testing/debugging code for every thing that needs to be done
  /*
  int int_number = 2;
  int int_reply = perf.SendInt(int_number);
  cout << "Integer Received: " << int_reply << endl;
  cout << endl;

  double double_num = 4.0;
  double double_reply = perf.SendDouble(double_num);
  cout << "Double Received: " << double_reply << endl;
  cout << endl;

  string stringmessage("Justin is awesome!!");
  string string_reply = perf.SendString(stringmessage);
  cout << string_reply << endl;
  cout << endl;

  Student student(3.5, 4);
  string student_reply = perf.GetCollegeAdmission(student);
  cout << student_reply << endl;
  cout << endl;

  perf.SendLargeString(1000000);
  cout << endl;

  //server streaming
  perf.ListColleges();
  cout << endl;

  //client streaming
  perf.SendPreferences(1000000);
  cout << endl;
  */
  return 0;
}
