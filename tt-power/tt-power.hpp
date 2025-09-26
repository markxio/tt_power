#include <iostream>
#include <fstream>
#include <sys/time.h>

namespace tt_power {

static double getTiming(long, long);
static long getEpoch();

static long getEpoch() {
  struct timeval tm;
  gettimeofday(&tm, NULL);
  return (tm.tv_sec * 1000000)+tm.tv_usec;
}

static double getTiming(long end_time, long start_time) {
  return (end_time - start_time) / 1.0e3 ;
}

class TTP {
    private:
        int n;
        double avg;
        char *filename; // eg. /sys/bus/pci/devices/0000:65:00.0/hwmon/hwmon2/power1_input

        void add_sample(double p) {
            this->avg = (this->avg * ((double)this->n / (this->n+1)) + p * ((double)1 / (this->n+1)));
            this->n += 1;
        }

        int readIntFromFile(const char *filename) {
            std::ifstream infile(filename);
            int value;

            if (!infile) {
                std::cerr << "Failed to open file: " << filename << "\n";
                return -1;
            }

            infile >> value;
            if (infile.fail()) {
                std::cerr << "Failed to read integer from file: " << filename << "\n";
                return -1;
            }

            return value;
        }

    public:
        TTP(char *filename) {
            this->n = 0;
            this->avg = 0.0;
            this->filename = filename;
        }           
        ~TTP() {}

        double getPower(bool &poison) {
            while (!poison) {
                int p = this->readIntFromFile(this->filename);
                // 12000000 = 12W
                double power = p / 1000000.0; // Convert to watts
                this->add_sample(power);
            }
            return this->avg;
        }
};
} // namespace tt_power
