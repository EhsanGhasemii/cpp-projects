import sys
from pathlib import Path

BUILD_DIR = Path("/app/projects/cpp-binding/build/")

sys.path.insert(0, str(BUILD_DIR))

import mymodule

print("result: ", mymodule.add(2, 3))

