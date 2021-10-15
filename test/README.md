# Test Input and Output Data

Two folders in this directory, `input_data` and `output_data`, are used to interact with the test code.

`input_data` contains the test data required for testing.  
This folder should not be modified in any way to ensure the tests function as expected.

`output_data` contains any files output from the tests, for human inspection.

See below for specifications of what files are required/output for each test type.

## Non-MPI unit tests

All files included should be sufficient for running the non-MPI tests correctly.
These tests output an example log file.

## MPI unit tests

These tests do not require any input data, and do not output any data.

## Integration tests

All files included should be sufficient for running the integration tests correctly.

The integration tests will also generate temporary data and output data in the specified working directory; please see the main README for details.
