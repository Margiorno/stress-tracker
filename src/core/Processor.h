#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
public:
    virtual ~Processor() {}

    virtual void process() = 0;

    virtual bool requiresAggregation() = 0;
};

#endif