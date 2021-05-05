#include "processes/producer.h"
#include "processes/consumer.h"
#include "processes/modulator.h"
#include "supervisor/supervisor.h"

int main(int argc, char *argv[])
{
    supervisor Super;
    producer();
    producer();
    modulator();
    producer();
    consumer();
    modulator();
    consumer();
    producer();

    return 0;
};