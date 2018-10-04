To compile and run this performance test you first need to generate an input file using a third-party program:

javac ../fasta/fasta.java 
java -cp ../fasta fasta 25000000 > knucleotide-input25000000.txt

Then you compile and run the test:
wiseyc knucleotide.yz
time ./runnable < knucleotide-input25000000.txt
