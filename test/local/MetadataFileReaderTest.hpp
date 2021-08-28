#pragma once

#include "../TestHelper.hpp"

// Requires external files to run tests correctly.
// File location: /app/signals
// Files required: 1294797712.metafits
//                 1294797712_1294797712_118.sub
//
// File location: /app/signals/invalid_metafits
// Files required: empty metafits file, valid voltage file
//
// File location: /app/signals/invalid_voltage
// Files required: valid metafits file, empty voltage file

class MetadataFileReaderTest : public TestModule {
	public:
	    MetadataFileReaderTest();
};