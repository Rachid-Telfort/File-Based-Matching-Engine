# File-Based-Matching-Engine
File-based stock matching engine that will be used as a basis for a real-time version later on.

# Business Summary
In the finance world, trades are usually handled via a process of algorithms that aim to make the trades as fast and fair as possible. The software that typically handles and executes trades is a matching engine. Exchanges can have multiple matching engines where each matching engine hadles a particular instrument. For example, an exchange can have a matching engine that handles trading stocks while another handles trading options. Once a trade has been made, the matching engine sends the trade results to all the participants of the trade. To make trades as fair and as transparent as possible, certain rules are applied to orders in a matching engine such as orders are traded based on when they come in the matching engine, or the order with the highest buy price is traded with the order with the lowest sale price. One would have to refere to a particular exchange's documentation to see how it handles matching orders.

# Project Summary
The file-based matching engine is based on stocks since stocks are relatively straightforard to understand and is a good introduction to how trades work. Overall the file-based matching engine reads from a file, via the command line, with certain instructions and processes those instructions. While processing the instructions, the file-based matching engine outputs the results either to the console or to an output file, specified via the command line.

# Source Information
This repository contains the C++ sources files, a Windows executable, as well some .txt files containing format of the file containing instructions, the format of command line arguments, an example set of instructions, as well as example output.

Currently, the program only runs on Windows since this project is a stepping stone to a more real-time version that will be implemented later on. I figured I would share the knowledge gained through this project with my fellow programmers which is why I'm uploading this. The real-time version will be built for both Windows and Linux.

The program was compiled using g++ 12.1.0 using the Code::Blocks IDE with C++ 11 on Windows. If one wishes to compile the source code, they would need to copy the source files to their own IDE or code base and compile and link using any C++11-compliant compiler.

# Final Notes
This project was inspired by this HackerRank coding challenge: [Equity Order Matching](https://www.hackerrank.com/contests/gs-codesprint-2018/challenges/equity-order-matching/problem)

I hope my fellow programmers find this informative and useful for their needs! I appreciate anyone that checks out this page!
