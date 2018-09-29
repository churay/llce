#!/bin/bash

# This is just a very basic script that launches a "virtual environment"
# with Address Space Layout Randomization (ASLR) disabled, which allows
# embedded processes to have more routine addressing and (consequently)
# easier debug behavior.

echo "Entering Debug Env"
setarch `uname -m` -R /bin/bash
echo "Exiting Debug Env"
