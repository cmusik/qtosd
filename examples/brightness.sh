#!/bin/awk -f

# This script works with a Thinkpad T61p

BEGIN{
	getline current < "/sys/class/backlight/acpi_video0/actual_brightness"
	getline max < "/sys/class/backlight/acpi_video0/max_brightness"
	print current "/" max "%Brightness" | "nc -c localhost 5000"
}
