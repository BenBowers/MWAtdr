#pragma once

#include "../TestHelper.hpp"

// Requires external files to run tests correctly.
// File location: /mnt/input
// Files required: 1294797712.metafits
//                 1294797712_1294797712_118.sub
//
// File location: /mnt/input/invalid_metafits
// Files required: empty metafits file, valid voltage file
//
// File location: /mnt/input/invalid_voltage
// Files required: valid metafits file, empty voltage file(s)
//
// File location: /mnt/input/no_voltage
// Files required: valid metafits file, no voltage files

TestModule metadataFileReaderTest();