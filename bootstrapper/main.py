import os


class SourceFileNotFoundException(Exception):
    def __init__(self, message):
        super().__init__(message)


def file_exists(dir: str, path: str) -> bool:   
    return path in os.listdir(dir)


def main():
    # directory to search for src in (should start at cwd)
    _dir_path = os.getcwd()

    # src file currently being parsed (should start at main.blam or entry)
    _src_path = 'main.blam'

    # parse main.blam
    if file_exists(_dir_path, _src_path):
        print("t")
    else:
        raise SourceFileNotFoundException(f'{_src_path} not found in {_dir_path}')


if __name__ == '__main__':
    main()

"""
1. locate from entry point (should start with main.blam in cwd)
2. tokenize
3. parse imports and module imports from rest of code
3.1. parse modules used in non-module
4. build linker tree
    recurse until no imports found
    1. locate from entry point (should be linked file)
    2. tokenize
    3. parse imports (should adhere to parsing rules)
"""