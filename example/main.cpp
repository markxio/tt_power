#include "tt_power.hpp"
#include "matmul.hpp"
#include <omp.h>

int main() {

    bool stop_measurement = false;
    double avg_power = 0.0;

	double start=0.0;
	double total_time=0.0; 

	// file exposing the power value at a time
    char filename[] = "/sys/bus/pci/devices/0000:65:00.0/hwmon/hwmon2/power1_input";
    tt_power::TTP ttp(filename);

    #pragma omp parallel shared(stop_measurement, avg_power, start, total_time) num_threads(2)
    {
        int tid = omp_get_thread_num();
        if(tid == 0) {
            // Perform some computation...
            std::vector<std::vector<int>> m1 = { {1, 1}, {2, 2} };
            std::vector<std::vector<int>> m2 = { {1, 1}, {2, 2} };
            std::vector<std::vector<int>> res;

			start=tt_power::getEpoch();
			for (int i=0; i<100; i++) {
            	matMul(m1, m2, res);
			}
			// once done, stop measurement
            stop_measurement=true;
			total_time=tt_power::getTiming(tt_power::getEpoch(), start);
           
        } else {
            avg_power = ttp.getPower(stop_measurement);
        }
    }

    printf("Total time: %.3f s, avg card power: %.3f W\n", total_time, avg_power);
    return 0;
}
