#!/bin/bash

# NOTE(JRC): This script will intake an arbitrary input binary and
# a destination path and create a new script at the desination that
# is a version of the binary with Address Space Layout Randomization
# (ASLR) disabled. The purpose of disabling this feature is to make
# it easier for an output process to use static address during debugging
# (which is a big piece in enabling loop-live code editing).

SRC_BINARY=${1}
DST_BINARY=${2}

(echo "#!/bin/bash"; echo "setarch `uname -m` -R ${SRC_BINARY}") > ${DST_BINARY}
chmod u+rwx ${DST_BINARY}
