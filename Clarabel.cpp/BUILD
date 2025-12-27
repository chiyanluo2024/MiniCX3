load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

config_setting(
    name = "linux_build",
    flag_values = {":buildtype": "linux"},
)

config_setting(
    name = "windows_build",
    flag_values = {":buildtype": "windows"},
)

string_flag(
	name = "buildtype",
	build_setting_default = "linux",
)

cc_import(
	name = "Clarabel.cpp-staticlib",
	hdrs = glob(["include/**"]),
	static_library = select({
                ":linux_build": "libs/libclarabel_c.a",
                ":windows_build": "libs/clarabel_c.lib",
        }),
)

cc_import(
        name = "Clarabel.cpp-sharedlib",
        interface_library = select({
		":linux_build": None,
                ":windows_build": "libs/clarabel_c.dll.lib",
        }),
        shared_library = select({
                ":linux_build": "libs/libclarabel_c.so",
                ":windows_build": "libs/clarabel_c.dll",

        }),
	deps = [":Clarabel.cpp-staticlib"]
)


cc_library(
    name = "Clarabel.cpp",
    deps = [":Clarabel.cpp-sharedlib"],
    includes = ["include"],
    visibility = ["//visibility:public"],
)
