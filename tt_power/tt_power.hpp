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
		bool writeProfile;
		char *profileFilename;
        char *filename; // eg. /sys/bus/pci/devices/0000:65:00.0/hwmon/hwmon2/power1_input

        void addSample(double p) {
            this->avg = (this->avg * ((double)this->n / (this->n+1)) + p * ((double)1 / (this->n+1)));
            this->n += 1;
			if(this->writeProfile) {
				this->writeToFile(this->avg);
			}
        }

        int readIntFromFile(const char *filename) {
            std::ifstream infile(filename);
            int value;

            if (!infile) {
                std::cerr << "Failed to open file: " << filename << "\n";
            }

            infile >> value;
            if (infile.fail()) {
                std::cerr << "Failed to read integer from file: " << filename << "\n";
            }

            return value;
        }

		void createFile(char *filename) {
			std::ofstream outFile(filename);
			
			if (!outFile) {
	    	    std::cerr << "Error: could not open file for writing.\n";
		    }

			outFile << "power_watt" << std::endl;
		}

		void writeToFile(double value) {
			std::ofstream outFile(this->profileFilename, std::ios::app);
			
			if (!outFile) {
	    	    std::cerr << "Error: could not open file for writing.\n";
		    }

			outFile << std::fixed;
            outFile << std::setprecision(3);
			outFile << value << std::endl;
		}

    public:

		TTP(char *filename) {
            this->reset();
            this->filename = filename;
			this->writeProfile = false;
        }

        ~TTP() {}

        double getPower(bool &poison) {
            while (!poison) {
                int p = this->readIntFromFile(this->filename);
                // 12000000 = 12W
                double power = p / 1000000.0; // Convert to watts
                this->addSample(power);
            }
            return this->avg;
        }

		void reset() {
			this->n = 0;
			this->avg = 0.0;
		}

		void enableProfiling(char *filename) {
			this->writeProfile = true;
			this->profileFilename = filename;
			this->createFile(filename);
		}
};
} // namespace tt_power
