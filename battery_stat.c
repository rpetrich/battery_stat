#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <stdint.h>

struct battery_info {
	int32_t current_capacity;
	int32_t max_capacity;
	bool external_connected;
	bool fully_charged;
};

static bool get_battery_info(struct battery_info *battery_info, bool print_debugging) {
	bool result = false;
	io_service_t batteryService = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleSmartBattery"));
	if (batteryService) {
		CFMutableDictionaryRef properties = NULL;
		if (IORegistryEntryCreateCFProperties(batteryService, &properties, NULL, 0) == 0 && properties != NULL) {
			if (print_debugging) {
				CFStringRef description = CFCopyDescription(properties);
				CFDataRef data = CFStringCreateExternalRepresentation(kCFAllocatorDefault, description, kCFStringEncodingUTF8, 0);
				CFRelease(description);
				fprintf(stderr, "%.*s\n", (int)CFDataGetLength(data), CFDataGetBytePtr(data));
				CFRelease(data);
			}
			result = true;
			result &= CFNumberGetValue(CFDictionaryGetValue(properties, CFSTR("CurrentCapacity")), kCFNumberSInt32Type, &battery_info->current_capacity);
			result &= CFNumberGetValue(CFDictionaryGetValue(properties, CFSTR("MaxCapacity")), kCFNumberSInt32Type, &battery_info->max_capacity);
			const void *external_connected = CFDictionaryGetValue(properties, CFSTR("ExternalConnected"));
			if (external_connected) {
				battery_info->external_connected = CFBooleanGetValue(external_connected);
			} else {
				result = false;
			}
			const void *fully_charged = CFDictionaryGetValue(properties, CFSTR("FullyCharged"));
			if (fully_charged) {
				battery_info->fully_charged = CFBooleanGetValue(fully_charged);
			} else {
				result = false;
			}
			CFRelease(properties);
		}
		IOObjectRelease(batteryService);
	}
	return result;
}


int main(int argc, const char *argv[]) {
	struct battery_info result;
	if (get_battery_info(&result, false)) {
		if (argc == 2 && strcmp(argv[1], "prompt") == 0) {
			const char *prefix;
			const char *suffix;
			if (result.fully_charged) {
				prefix = "";
				suffix = "";
			} else {
				int32_t percent = (result.current_capacity * 100) / result.max_capacity;
				if (percent >= 70) {
					prefix = "\x1b[32m";
				} else if (percent >= 30) {
					prefix = "\x1b[1;33m";
				} else {
					prefix = "\x1b[31m";
				}
				suffix = "\x1b[00m";
			}
			printf(result.external_connected ? "%s$%s" : "%s!%s", prefix, suffix);
		} else if (argc == 1 || (argc == 2 && strcmp(argv[1], "simple") == 0)) {
			printf("current_capacity=%d max_capacity=%d external_connected=%d fully_charged=%d\n", result.current_capacity, result.max_capacity, result.external_connected, result.fully_charged);
		} else if (argc == 2 && strcmp(argv[1], "json") == 0) {
			printf("{\"current_capacity\":%d,\"max_capacity\":%d,\"external_connected\":%s,\"fully_charged\":%s}\n", result.current_capacity, result.max_capacity, result.external_connected ? "true" : "false", result.fully_charged ? "true" : "false");
		} else {
			fprintf(stderr, "usage:\n\tbattery_stat\n\tbattery_stat simple\n\tbattery_stat json\n\tbattery_stat prompt\n");
			return 1;
		}
		return 0;
	} else {
		fprintf(stderr, "failed to retrieve battery info\n");
		return 1;
	}
}