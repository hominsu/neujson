variable "REPO" {
  default = "hominsu"
}

variable "AUTHOR_NAME" {
  default = "hominsu"
}

variable "AUTHOR_EMAIL" {
  default = "hominsu@foxmail.com"
}

variable "ALPINE_VERSION" {
  default = "3.16"
}

variable "VERSION" {
  default = ""
}

group "default" {
  targets = [
    "neujson-alpine",
  ]
}

target "neujson-alpine" {
  context    = "."
  dockerfile = "docker/Dockerfile"
  args       = {
    AUTHOR_NAME    = "${AUTHOR_NAME}"
    AUTHOR_EMAIL   = "${AUTHOR_EMAIL}"
    ALPINE_VERSION = "${ALPINE_VERSION}"
    VERSION        = "${VERSION}"
  }
  tags = [
    "${REPO}/neujson:alpine-${ALPINE_VERSION}-latest",
    notequal("", VERSION) ? "${REPO}/neujson:alpine-${ALPINE_VERSION}-${VERSION}" : "",
  ]
  platforms = ["linux/amd64"]
}