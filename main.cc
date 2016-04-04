#include <cerrno>
#include <cstring>
#include <gflags/gflags.h>

#include "controller.h"
#include "driver.h"
#include "program.h"
#include "sequence_generator.h"
#include "status.h"

DEFINE_bool(two_pin_programming, true, "Enable two-pin single-supply-voltage (LVP) programming.");

int main(int argc, char **argv) {
	google::ParseCommandLineFlags(&argc, &argv, true);

	// FIXME: set this based on the chip type.
	std::unique_ptr<Pic18SequenceGenerator> sequence_generator;
	if (FLAGS_two_pin_programming) {
		sequence_generator.reset(new KeySequenceGenerator);
	} else {
		sequence_generator.reset(new PgmSequenceGenerator);
	}
	std::unique_ptr<Driver> driver = Driver::CreateFromFlags();
	std::unique_ptr<Controller> controller(new Pic18Controller(std::move(driver), std::move(sequence_generator)));
	auto device_db = std::make_unique<DeviceDb>();
	CHECK_OK(device_db->Load());
	HighLevelController high_level_controller(std::move(controller), std::move(device_db));

	Program program;
	CHECK_OK(high_level_controller.ReadProgram(&program));
	FILE *out = fopen("dump.hex", "w+");
	WriteIhex(program, out);
	fclose(out);
/*

	CHECK_OK(controller->Open());

	uint16_t device_id;
	CHECK_OK(controller->ReadDeviceId(&device_id));
	// TODO: fetch information from the device ID database.
	printf("Device ID: %04x\n", device_id);

#if 0
	datastring program;
	for (int i = 0; i < 64; ++i) program.push_back(i);

	controller.WriteFlash(64, program);
	//controller.WriteFlash(0x7F80, program);
#else
#if 0
	controller.RowErase(0);
	controller.RowErase(64);
#else
	datastring program;
//	CHECK_OK(controller.ReadFlashMemory(0, 256, &program));
	CHECK_OK(controller->ReadFlashMemory(0, 0x8100, &program));
	FILE *out = fopen("dump.bin", "w+");
	if (!out) {
		FATAL("Could not open output file: %s", strerror(errno));
	}
	fwrite(program.data(), 1, program.size(), out);
	fclose(out);
#endif
#endif
*/
	return EXIT_SUCCESS;
}
