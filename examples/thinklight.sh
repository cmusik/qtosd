#!/bin/sh

awk '/^status:/ { if ($2 == "on") {s="c/ThinkLight on"} else {s="c/ThinkLight off"}; print s | "nc -c localhost 5000" }' /proc/acpi/ibm/light
