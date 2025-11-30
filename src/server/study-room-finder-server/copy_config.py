import os
import shutil

config_file = "src/config.hpp"
example_file = "src/config.example.hpp"

if not os.path.exists(config_file):
    shutil.copy(example_file, config_file)
    print(f"Created {config_file} from {example_file}")