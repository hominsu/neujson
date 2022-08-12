load("@bazel_skylib//lib:selects.bzl", "selects")

def _native():
    selects.config_setting_group(
        name = "ppc",
        match_any = [
            "@platforms//cpu:ppc",
        ],
    )

    selects.config_setting_group(
        name = "macos_aarch64_llvm",
        match_all = [
            "@platforms//os:macos",
            "@platforms//cpu:aarch64",
            "@bazel_tools//tools/cpp:clang",
        ],
    )

    return select({
        ":ppc": ["-mcpu=native"],
        ":macos_aarch64_llvm": ["-mcpu=apple-m1"],
        "//conditions:default": ["-march=native"],
    })

def _extra_error():
    basic = [
        "-Wall",
        "-Wextra",
        "-Werror",
        "-Weffc++",
        "-Wswitch-default",
        "-Wfloat-equal",
        "-Wconversion",
    ]
    return basic + select({
        "@bazel_tools//tools/cpp:clang": [
            "-Wimplicit-fallthrough",
            "-Wno-missing-field-initializers",
        ],
        "@bazel_tools//tools/cpp:gcc": [
            "-Wsign-conversion",
        ],
        "//conditions:default": [],
    })

def _copts(extra_error = True, native = False):
    copts = ["-std=c++17"]
    if native:
        copts += _native()
    if extra_error:
        copts += _extra_error()
    return copts

neujson = struct(
    copts = _copts,
)
