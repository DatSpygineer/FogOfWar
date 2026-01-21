import os.path
import shutil
import argparse

def parse_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("--input", required=True, help="Path to the folder to be packed")
	parser.add_argument("--output", required=True, help="Path to the output archive file")
	return parser.parse_args()

def print_error(*_args):
	print(f"[\x1B[31;1mERROR\x1B[0m] ", end="")
	print(*_args)

if __name__ == "__main__":
	args = parse_args()
	input_path = str(args.input)
	output_path = str(args.output)
	if not os.path.exists(input_path):
		print_error(f"Failed to package assets from \"{input_path}\": Directory not found!")
		exit(1)

	if not os.path.exists(os.path.split(output_path)[0]):
		os.makedirs(os.path.split(output_path)[0])

	if os.path.exists(output_path):
		os.remove(output_path)

	print(f"Packaging asset folder \"{input_path}\"")
	shutil.make_archive(os.path.splitext(output_path)[0], "zip", input_path)
	shutil.move(os.path.splitext(output_path)[0] + ".zip", output_path)
	print(f"Packed assets to \"{output_path}\"")