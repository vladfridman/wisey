Wisey language compiler 

## Installation instructions ##

Run the following where ~/wisey is the directory where you unpacked the downloaded archive: 

export WISEY_HOME=~/wisey
export PATH=$PATH:$WISEY_HOME/bin

You can optionally add these lines to your ~/.profile in order not to run them every time you want to use the Wisey compiler.

## Check compiler ##

compile the HelloWorld program and run it: 

wiseyc $WISEY_HOME/samples/HelloWorld/HelloWorld.yz -o helloworld
./helloworld

compile a multithreaded version of HelloWorld and run it: 

wiseyc $WISEY_HOME/samples/HelloWorldMultiThreaded/HelloWorldMultiThreaded.yz
./runnable

## Resources ##

Wisey library and language reference: http://wisey.info

You can also check out performance tests contained in the $WISEY_HOME/performance directory
