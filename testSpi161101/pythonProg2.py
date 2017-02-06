import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--foo', nargs=2)
parser.add_argument('--bar', nargs=1)
args = parser.parse_args()

#print args.foo[0]
print(args)