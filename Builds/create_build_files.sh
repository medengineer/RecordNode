#!/bin/bash
if [ -d VS2013 ]; then
	rm -rf VS2013
fi
mkdir VS2013 && cd VS2013
cmake -G "Visual Studio 12 2013 Win64" ..