import argparse

parser = argparse.ArgumentParser()
group = parser.add_mutually_exclusive_group()
group.add_argument("-v", "--verbose", action="store_true")
group.add_argument("-q", "--quiet", action="store_true")
group.add_argument("-s", "--sum", action="store_true")
parser.add_argument("x", type=int, help="the base")
parser.add_argument("y", type=int, help="the exponent")
parser.add_argument("z", type=int, help="the exponent")
args = parser.parse_args()
answer = args.x+args.y+args.z

if args.quiet:
	print x
	print y
	print answer
elif args.verbose:
	print "{} to the power {} equals {}".format(args.x, args.y, answer)
elif args.sum:
	print args.x
	print args.y
	print args.z
	print "{}+{}+{} == {}".format(args.x, args.y, args.z, answer)
	#print "{} to the power {} equals {}".format(args.x, args.y, answer)
else:
	print args.x
	print args.y
	print "{}+{}+{} == {}".format(args.x, args.y, answer)