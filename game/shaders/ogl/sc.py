import glob
from pathlib import Path
import sys
import os

class ShaderDesc:
    def __init__(self, name: str, ext: str, path: Path):
        self.name = name
        self.ext = ext
        self.path = path

class IncludeDesc:
    def __init__(self, data: str):
        self.data = data
        self.is_ready = False

def process(filename: str, includes: dict[Path, IncludeDesc]) -> IncludeDesc:
    filepath = Path(filename).parent

    if includes.get(filename) is None:
        desc = IncludeDesc("")
        includes[filename] = desc

        with open(filename, "r") as file:
            #<spaces*>#include "<name>"<spaces*>
            line_number: int = 1
            for orig_line in file:
                line = orig_line.strip()
                if not line.startswith("#include"):
                    desc.data += orig_line
                    line_number += 1
                    continue

                line = line.replace("#include", "", 1).lstrip()
                if not (line.startswith("\"") and line.endswith("\"")):
                    raise Exception(f"File: {filename}, line: {line_number}, Bad quotes")
                
                line = line.removeprefix("\"").removesuffix("\"")

                if line == "":
                    raise Exception(f"File: {filename}, line: {line_number}, Bad name")
                
                if os.path.isabs(line):
                    desc.data += process(line, includes).data
                else:
                    desc.data += process(filepath / line, includes).data

                line_number += 1
        
        desc.is_ready = True
    else:
        if includes[filename].is_ready == False:
            raise Exception(f"File: {filename}, Loop detected")
        
    return includes[filename]

#name source_path compiled_path
source_path = Path(sys.argv[1])
compiled_path = Path(sys.argv[2])

compiled_path.mkdir(parents=True, exist_ok=True)

extensions = ["vert", "frag"]
shaders: list[ShaderDesc] = []
for ext in extensions:
    files = glob.glob(f"{source_path / f"*.{ext}"}")
    for file in files:
        shaders.append(ShaderDesc(Path(file).stem, ext, os.path.abspath(file)))

includes: dict[Path, IncludeDesc] = {}

for shader in shaders:
    include = includes.get(shader.path)
    if include is None:
        include = process(shader.path, includes)

    with open(compiled_path / f"{shader.name}.{shader.ext}", "w+") as file:
        file.write(include.data)

    #print(f"Shader: {shader.name}.{shader.ext}")
    #print(f"Data:\n{include.data}")


                
                
