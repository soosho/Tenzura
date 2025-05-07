 #!/usr/bin/env bash

 # Execute this file to install the tenzura cli tools into your path on OS X

 CURRENT_LOC="$( cd "$(dirname "$0")" ; pwd -P )"
 LOCATION=${CURRENT_LOC%Tenzura-Qt.app*}

 # Ensure that the directory to symlink to exists
 sudo mkdir -p /usr/local/bin

 # Create symlinks to the cli tools
 sudo ln -s ${LOCATION}/Tenzura-Qt.app/Contents/MacOS/tenzurad /usr/local/bin/tenzurad
 sudo ln -s ${LOCATION}/Tenzura-Qt.app/Contents/MacOS/tenzura-cli /usr/local/bin/tenzura-cli
