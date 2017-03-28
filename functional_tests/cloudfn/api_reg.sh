#!/bin/bash

nebula delete-code embeddedSdkTest-1.0.0.tgz
nebula create-api api.yaml
nebula create-function function.yaml
nebula create-code embeddedSdkTest-1.0.0.tgz
