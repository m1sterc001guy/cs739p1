#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <iostream>

class Student {
  public:
    Student(double gpa, int num_of_classes){
      this->gpa = gpa;
      this->num_of_classes = num_of_classes;
    }
    
    double get_gpa() {
      return this->gpa;
    }

    int get_number_of_classes() {
      return this->num_of_classes;
    }
  private:
    double gpa;
    int num_of_classes;
};

#endif
