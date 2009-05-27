# $Header$
#
# Example use of option -k
#
# $Log$
# Revision 1.1  2009-05-27 15:59:37  tino
# Now works as designed
#

while ./runningfor 12
do
	while ./runningfor 1 a; do echo -n a; done

	./runningfor -k 7 &&
	while ./runningfor 2 b; do echo -n b; done

	echo
done

