# MWA Time Data Reconstructor Utility Library

## Summary

A Python library of utilities to assist interfacing with the MWA Time Data Reconstructor application.

## Usage

Add the `mwatdr_utils` directory to the Python module path, then:

```python
import mwatdr_utils
```

Useful operations include:

- Reading/writing inverse polyphase filter files: `mwatdr_utils.inv_polyphase_filter` module. `read_inv_polyphase_filter()` and `write_inv_polyphase_filter()` functions.
- Reading/writing output time domain signal files: `mwatdr_utils.output_signal` module. `read_output_signal()` and `write_output_signal()` functions.

For more information and proper documentation, please see the source code in the `mwatdr_utils` directory.

## Requirements

- Python 3. Targeted version 3.8.2.
- Numpy. Targeted version 1.18.0.
- (For running tests) Pytest. Targeted version 4.6.9

Note that the targeted versions are only what the software was developed with, and slightly older/newer versions of requirements are likely to work too.

## Testing

To run the tests, run `python3 -m pytest` in this directory.
