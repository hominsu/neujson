workspace(name = "pers_hominsu_neujson")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
    ],
    sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
)

http_archive(
  name = "com_google_googletest",
  urls = [
      "https://github.com/google/googletest/archive/release-1.12.1.tar.gz",
      "https://gitee.com/mirrors/googletest/repository/archive/release-1.12.1.tar.gz"
  ],
  sha256 = "81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2",
  strip_prefix = "googletest-release-1.12.1",
)