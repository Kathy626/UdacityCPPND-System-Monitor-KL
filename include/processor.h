#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor
{
public:
    Processor();
    float Utilization(); // TODO: See src/processor.cpp

    // added two prev values memebers to use for utilization calculation
private:
    long PrevTotal_ = 0;
    long PrevIdle_ = 0;
};

#endif