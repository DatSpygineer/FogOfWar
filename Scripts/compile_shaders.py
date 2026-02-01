import os
import json
import argparse

class ShaderInfo:
	def __init__(self, name: str, vertex: str, fragment: str):
		self.name     = name
		self.vertex   = vertex
		self.fragment = fragment

SHADERLIB_SOURCE = """#include "ShaderLib.hpp"

static std::unordered_map<std::string, std::string> s_sources;
static std::unordered_map<std::string, ShaderSources> s_shaders;

void ShaderLibInitialize(void(*register_fn)(const char*, const char*)) {
$(SHADER_REGISTER)
}
const char* ShaderLibGetSource(const char* name) {
    if (s_sources.contains(name)) {
        return s_sources.at(name).c_str();
    }
    return nullptr;
}
bool ShaderLibGetRequiredSources(const char* name, ShaderSources* sources) {
    if (sources != nullptr && s_shaders.contains(name)) {
        *sources = s_shaders.at(name);
        return true;
    }
    return false;
}
"""

def parse_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("--engine-shaders", required=True)
	parser.add_argument("--game-shaders")
	parser.add_argument("--source-out", required=True)
	return parser.parse_args()

def list_src_recursive(dir: str, rel: str) -> dict[str, str]:
	result = { }
	for path in os.listdir(dir):
		full_path = os.path.join(dir, path)
		rel_path  = os.path.join(rel, path)
		if os.path.isdir(full_path):
			dir_content = list_src_recursive(full_path, rel_path)
			for k, v in dir_content.items():
				result[k] = v
		else:
			with open(full_path, "r") as f:
				result[rel_path] = f.read()
	return result

def parse_shaders_json(json_file: str, source_files: dict[str, str], shaders: dict[str, ShaderInfo]):
	base_dir = os.path.split(json_file)[0]
	json_data = dict(json.load(open(json_file, "r")))
	if "include_dirs" in json_data.keys():
		for path in json_data["include_dirs"]:
			full_path = str(os.path.join(base_dir, path))
			for k, v in list_src_recursive(full_path, "").items():
				source_files[k] = v
	if "shaders" in json_data.keys():
		for name, shader in json_data["shaders"].items():
			try:
				rel_vertex_path  = shader["vertex"]
				full_vertex_path = os.path.join(base_dir, rel_vertex_path)
				with open(full_vertex_path, "r") as f:
					source_files[rel_vertex_path] = f.read()

				rel_fragment_path  = shader["fragment"]
				full_fragment_path = os.path.join(base_dir, rel_fragment_path)
				with open(full_fragment_path, "r") as f:
					source_files[rel_fragment_path] = f.read()
				shaders[name] = ShaderInfo(name, rel_vertex_path, rel_fragment_path)
			except Exception as e:
				print(f"\x1B[31mERROR\x1B[0m: Error loading shader \"{name}\" from {json_file}: {e}!")
				exit(1)
	else:
		print(f"\x1B[31mERROR\x1B[0m: No shaders specified in {json_file}!")
		exit(1)

def make_cstr(input: str) -> str:
	return (input
			  .replace('\\', r'\\')
			  .replace('\n', r'\n')
			  .replace('\r', r'\r')
			  .replace('"', r'\"')
			  .replace("'", r"\'")
			)

if __name__ == "__main__":
	args = parse_args()

	output_path = str(args.source_out)
	if not os.path.exists(output_path):
		os.makedirs(output_path)

	engine_shaders_json = os.path.join(args.engine_shaders, "shaders.json")
	if not os.path.exists(engine_shaders_json):
		print(f"\x1B[31mERROR\x1B[0m: Engine shader definition \"{engine_shaders_json}\" cannot be found!")
		exit(1)

	source_files_result = { }
	shaders_result      = { }
	parse_shaders_json(engine_shaders_json, source_files_result, shaders_result)

	if args.game_shaders:
		game_shaders_json = str(os.path.join(args.game_shaders, "shaders.json"))
		if os.path.exists(game_shaders_json):
			parse_shaders_json(game_shaders_json, source_files_result, shaders_result)

	source_path = os.path.join(output_path, "ShaderLib.cpp")

	with (open(source_path, "w") as f):
		shader_fill_map = ""
		shader_register = ""
		for name, source in source_files_result.items():
			source_cstr = make_cstr(source)
			shader_fill_map += \
f'''    if (s_sources.contains("{name}"))  {{
        s_sources.at("{name}") = "{source_cstr}";
    }} else {{
        s_sources.emplace("{name}", "{source_cstr}");
    }}
'''
			shader_register += f'    if (register_fn != nullptr) register_fn("/{name}", "{source_cstr}");\n'

		for name, shader in shaders_result.items():
			shader_register += \
f'''
    if (s_shaders.contains("{name}"))  {{
        s_shaders.at("{name}") = ShaderSources("{shader.vertex}", "{shader.fragment}");
    }} else {{
        s_shaders.emplace("{name}", ShaderSources("{shader.vertex}", "{shader.fragment}"));
    }}'''

		f.write(SHADERLIB_SOURCE.replace("$(SHADER_REGISTER)", f"{shader_fill_map}\n{shader_register}"))
		print(f"Written shaderlib source file \"{source_path}\"")