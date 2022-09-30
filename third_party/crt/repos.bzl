load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def crt_repos(local=None):
    if local:
        native.local_repository(
            name = "crt",
            path = local,
        )
    else:
        maybe(
            http_archive,
            name = "crt",
            sha256 = "4b5020a02cc6a51014307f886b5619c4e19ac67879807d4e00fa71dc97a29735",
            url = "https://github.com/lowRISC/crt/archive/refs/tags/v0.3.5.tar.gz",
            strip_prefix = "crt-0.3.5",
        )
