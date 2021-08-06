#include "OutputLogFileWriter.hpp"

int main() {
	AppConfig app;
	ChannelRemapping remapping;
	ObservationProcessingResults results;
	AntennaConfig antenna;
	
	writeLogFile(app, remapping, results, antenna);
}
