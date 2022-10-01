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
            sha256 = "152669e31463e0a3931aa235515ab61337f915a56b931d95b7b17c3783a7fbaa",
            url = "https://github.com/cfrantz/crt/archive/refs/tags/v0.3.5-p1.tar.gz",
            strip_prefix = "crt-0.3.5-p1",
        )
